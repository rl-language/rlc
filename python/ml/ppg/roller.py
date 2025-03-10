from collections import defaultdict

import numpy as np
import torch as th

from . import torch_util as tu
from . import tree_util
from . import vec_monitor2


class Roller:
    def __init__(
        self,
        *,
        venv: "(VecEnv)",
        act_fn: "ob, state_in, first -> action, state_out, dict",
        initial_state: "RNN state",
        keep_buf: "number of episode stats to keep in rolling buffer" = 100,
        keep_sep_eps: "keep buffer of per-env episodes in VecMonitor2" = False,
        keep_non_rolling: "also keep a non-rolling buffer of episode stats" = False,
        keep_cost: "keep per step costs and add to segment" = False,
        past_stragey_model = None,
    ):
        """
        All outputs from public methods are torch arrays on default device
        """
        self._act_fn = act_fn
        if not isinstance(venv, vec_monitor2.VecMonitor2):
            venv = vec_monitor2.VecMonitor2(
                venv,
                keep_buf=keep_buf,
                keep_sep_eps=keep_sep_eps,
                keep_non_rolling=keep_non_rolling,
            )
        self._venv = venv
        self._step_count = 0
        self._state = initial_state
        self._infos = None
        self._keep_cost = keep_cost
        self.has_non_rolling_eps = keep_non_rolling
        self.past_stragey_model = past_stragey_model
        self.current_past_strategy_player = 0

    @property
    def interact_count(self) -> int:
        return self.step_count * self._venv.num

    @property
    def step_count(self) -> int:
        return self._step_count

    @property
    def episode_count(self) -> int:
        return self._venv.epcount

    @property
    def recent_episodes(self) -> list:
        return self._venv.ep_buf.copy()

    @property
    def recent_eplens(self) -> list:
        return [ep.len for ep in self._venv.ep_buf]

    @property
    def recent_eprets(self) -> list:
        return [ep.ret for ep in self._venv.ep_buf]

    def recent_eprets_player(self, player_id) -> list:
        return [ep.player_ret[player_id] for ep in self._venv.ep_buf]

    @property
    def recent_epinfos(self) -> list:
        return [ep.info for ep in self._venv.ep_buf]

    @property
    def per_env_episodes(self) -> list:
        return self._venv.per_env_buf

    @property
    def non_rolling_eplens(self) -> list:
        if self._venv.non_rolling_buf is None:
            return None
        return [ep.len for ep in self._venv.non_rolling_buf]

    def recent_stats(self, stat_id) -> list:
        return [ep.extra_metrics[stat_id] for ep in self._venv.ep_buf]

    @property
    def non_rolling_eprets(self) -> list:
        if self._venv.non_rolling_buf is None:
            return None
        return [ep.ret for ep in self._venv.non_rolling_buf]

    @property
    def non_rolling_epinfos(self) -> list:
        if self._venv.non_rolling_buf is None:
            return None
        return [ep.info for ep in self._venv.non_rolling_buf]

    def clear_episode_bufs(self):
        self._venv.clear_episode_bufs()

    def clear_per_env_episode_buf(self):
        self._venv.clear_per_env_episode_buf()

    def clear_non_rolling_episode_buf(self):
        self._venv.clear_non_rolling_episode_buf()

    @staticmethod
    def sort_by_player(tensors) -> dict:
        out = defaultdict(list)

        player_ids = tensors["player_id"].reshape(-1)
        tensors.pop("player_id")
        return tensors
        sort_order = th.argsort(player_ids, stable=True)

        for key, value in tensors.items():
            original_shape = value.shape
            reshaped_tensor = value.reshape(-1, *value.shape[2:])
            sorted = reshaped_tensor[sort_order]
            out[key] = sorted.reshape(original_shape)
        return out

    @staticmethod
    def singles_to_multi(single_steps) -> dict:
        """
        Stack single-step dicts into arrays with leading axes (batch, time)
        """
        out = defaultdict(list)
        for d in single_steps:
            for k, v in d.items():
                out[k].append(v)

        # TODO stack
        def toarr(xs):
            if isinstance(xs[0], dict):
                return {k: toarr([x[k] for x in xs]) for k in xs[0].keys()}
            if not tu.allsame([x.dtype for x in xs]):
                raise ValueError(
                    f"Timesteps produced data of different dtypes: {set([x.dtype for x in xs])}"
                )
            if isinstance(xs[0], th.Tensor):
                return th.stack(xs, dim=1).to(device=tu.dev())
            elif isinstance(xs[0], np.ndarray):
                arr = np.stack(xs, axis=1)
                return tu.np2th(arr)
            else:
                raise NotImplementedError

        return {k: toarr(v) for (k, v) in out.items()}

    def multi_step(self, nstep, **act_kwargs) -> dict:
        """
        step vectorized environment nstep times, return results
        final flag specifies if the final reward, observation,
        and first should be included in the segment (default: False)
        """
        if self._venv.num == 0:
            self._step_count += nstep
            return {}
        state_in = self.get_state()
        singles = [self.single_step(**act_kwargs) for i in range(nstep)]
        out = self.singles_to_multi(singles)
        out = self.sort_by_player(out)
        out["state_in"] = state_in
        finalrew, out["finalob"], out["finalfirst"] = tree_util.tree_map(
            tu.np2th, self._venv.observe()
        )
        out["finalstate"] = self.get_state()
        out["finalmask"] = th.from_numpy(self._venv.action_mask()).to(tu.dev())
        out["reward"] = th.cat([out["lastrew"][:, 1:], finalrew[:, None]], dim=1)
        if self._keep_cost:
            out["finalcost"] = tu.np2th(
                np.array([i.get("cost", 0.0) for i in self._venv.get_info()])
            )
            out["cost"] = th.cat(
                [out["lastcost"][:, 1:], out["finalcost"][:, None]], dim=1
            )
        del out["lastrew"]
        return out

    def _resolve_players(self):
        for i in range(4):
            player = (self.current_past_strategy_player + i) % self._venv.get_num_player()
            while self._venv.current_player_one(i) != player:
                lastrew, ob, first =  tree_util.tree_map(tu.np2th, self._venv.observe_one(i))
                action_mask = th.from_numpy(self._venv.one_action_mask(i)).to(device=tu.dev())
                ac, newstate, other_outs = self.past_stragey_model.act(
                    ob=ob,
                    first=first,
                    state_in=self._state,
                    action_mask=action_mask,
                )

                self._venv.step_one(i, ac[0])

    def single_step(self, **act_kwargs) -> dict:
        """
        step vectorized environment once, return results
        """
        if self.past_stragey_model != None:
            self._resolve_players()

        out = {}
        lastrew, ob, first = tree_util.tree_map(tu.np2th, self._venv.observe())
        if self._keep_cost:
            out.update(
                lastcost=tu.np2th(
                    np.array([i.get("cost", 0.0) for i in self._venv.get_info()])
                )
            )
        current_player = tu.np2th(self._venv.current_player()).to(device=tu.dev())
        action_mask = th.from_numpy(self._venv.action_mask()).to(device=tu.dev())
        ac, newstate, other_outs = self._act_fn(
            ob=ob,
            first=first,
            state_in=self._state,
            action_mask=action_mask,
            **act_kwargs,
        )
        self._state = newstate
        out.update(
            lastrew=lastrew,
            ob=ob,
            first=first,
            ac=ac,
            action_mask=action_mask,
            player_id=current_player,
        )
        self._venv.act(tree_util.tree_map(tu.th2np, ac))
        for k, v in other_outs.items():
            out[k] = v
        self._step_count += 1
        return out


    def get_state(self):
        return self._state

    def log_extra_metrics(self, metric):
        return self._venv.log_extra_metrics(metric)

    def get_user_defined_log_functions(self):
        return self._venv.get_user_defined_log_functions()

    def observe(self):
        return self._venv.observe()
