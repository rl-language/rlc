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
            for (k, v) in d.items():
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

    def single_step(self, **act_kwargs) -> dict:
        """
        step vectorized environment once, return results
        """
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
            ob=ob, first=first, state_in=self._state, action_mask=action_mask, **act_kwargs
        )
        self._state = newstate
        out.update(lastrew=lastrew, ob=ob, first=first, ac=ac, action_mask=action_mask, player_id=current_player)
        self._venv.act(tree_util.tree_map(tu.th2np, ac))
        for (k, v) in other_outs.items():
            out[k] = v
        self._step_count += 1
        return out



    # def collect_steps(self, nstep, **act_kwargs) -> dict:
        # """
        # Collect steps for nstep timesteps without final environment observation.
        # Returns a combined dictionary of data shaped (nenv, nstep, ...).
        # """
        # if self._venv.num == 0:
            # self._step_count += nstep
            # return {}

        # state_in = self.get_state()
        # singles = [self.single_step(**act_kwargs) for _ in range(nstep)]
        # combined = self.singles_to_multi(singles)
        # combined["state_in"] = state_in

        # Convert lastrew into reward by shifting:
        # In single-agent code, usually reward = cat(lastrew[:,1:], finalrew[:,None])
        # Here we do not have a finalrew from observe at the end,
        # so just treat lastrew as the reward at that step.
        # reward for step t is lastrew at step t, there's no extra step after last step.
        # So reward == lastrew is fine here.
        # If needed, you can rename lastrew to reward for clarity.
        # combined["reward"] = combined["lastrew"]
        # if self._keep_cost:
            # combined["cost"] = combined["lastcost"]

        # return combined

    # def split_player_data(self, combined: dict) -> dict:
        # """
        # After collecting steps, split by player.
        # The finalstate, finalobs, finalmask, finalreward for each player is the last step they took.
        # """
        # player_id = combined["player_id"]  # shape (nenv, nstep)
        # nenv, nstep = player_id.shape

        # unique_players = th.unique(player_id)

        # # For each player, we find the steps they acted in.
        # # We'll keep data in (nenv, nstep) form and just mask it.
        # per_player_out = {}

        # # Identify keys that have a time dimension (like ob, ac, reward)
        # # We'll assume all keys except "state_in" are time-based (since collected each step)
        # time_based_keys = [k for k, v in combined.items() if k != "state_in" and k != "newstate" and v.dim() >= 2 and v.shape[1] == nstep]
        # # Note: keys like player_id, ob, ac, reward, first, action_mask, newstate should all be (nenv, nstep, ...).

        # for p in unique_players:
            # p_id = p.item()
            # # Create a mask for steps belonging to this player:
            # mask = (player_id == p_id)  # (nenv, nstep) boolean

            # # Extract all the steps for this player
            # # Flatten them into a single list of steps (like a single-agent trajectory)
            # # We'll gather along both env and time dimensions:
            # idx = mask.nonzero(as_tuple=False)  # shape (X, 2) where X is number of steps for this player
            # # idx[:,0] = env indices, idx[:,1] = step indices
            # # Steps are collected in order, so sorting by env and time is stable
            # # They should already be in order since we acted in steps in sequence.

            # # Gather data for each key
            # player_data = {}
            # for k in time_based_keys:
                # # shape: (nenv, nstep, ...)
                # v = combined[k]
                # # We gather using idx to get only the player's steps
                # player_data[k] = v[idx[:, 0], idx[:, 1]].reshape(v.shape[0], -1)

            # # Also store state_in for reference (same for all steps, if needed)
            # player_data["state_in"] = combined["state_in"]

            # # Now determine final step for this player: the last entry in idx
            # if idx.shape[0] > 0:
                # last_env = idx[-1, 0].item()
                # last_t = idx[-1, 1].item()

                # # finalobs: the observation at the last step they took
                # # finalmask: the action_mask at the last step
                # # finalreward: the reward at the last step
                # # finalstate: the newstate at the last step

                # player_data["finalob"] = combined["ob"][last_env, last_t].unsqueeze(0)
                # player_data["finalmask"] = combined["action_mask"][last_env, last_t].unsqueeze(0)
                # player_data["finalrew"] = combined["reward"][last_env, last_t].unsqueeze(0)
                # player_data["finalstate"] = combined["newstate"]
            # else:
                # # This player had no steps in this segment
                # # final arrays don't apply
                # player_data["finalob"] = None
                # player_data["finalmask"] = None
                # player_data["finalrew"] = None
                # player_data["finalstate"] = None

            # per_player_out[p_id] = player_data

        # return per_player_out

    # def multi_step(self, nstep, **act_kwargs) -> dict:
        # """
        # Step vectorized environment nstep times, then split by players,
        # using the last step a player took as their final state.
        # """
        # combined = self.collect_steps(nstep, **act_kwargs)
        # per_player_data = self.split_player_data(combined)
        # return per_player_data


    def get_state(self):
        return self._state

    def log_extra_metrics(self, metric):
        return self._venv.log_extra_metrics(metric)
    def get_user_defined_log_functions(self):
        return self._venv.get_user_defined_log_functions()

    def observe(self):
        return self._venv.observe()
