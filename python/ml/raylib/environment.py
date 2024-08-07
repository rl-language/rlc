from ray.rllib.env.multi_agent_env import MultiAgentEnv
import torch
import sys
from gymnasium import spaces
from gymnasium.spaces import Dict
import numpy as np
import random
from rlc import Program, State


class Specs:
    def __init__(self, max_steps=100):
        self.max_episode_steps = max_steps
        self.id = random.randint(0, 10)


def exit_on_invalid_env(sim, forced_one_player=False, needs_score=True):
    errors = validate_env(
        sim, forced_one_player=forced_one_player, needs_score=needs_score
    )
    if len(errors) == 0:
        return

    for error in errors:
        print(error)
    exit(-1)


def has_score_function(module):
    return hasattr(module, "rl_score__Game_int64_t_r_double")


def has_score(module):
    return has_score_function(module) or hasattr(module.Game(), "score")


def has_max_lenght(module):
    return hasattr(module, "rl_max_game_lenght__r_int64_t")


def has_get_num_players(module):
    return hasattr(module, "rl_get_num_players__r_int64_t")


def has_get_num_players(module):
    return hasattr(module, "rl_get_num_players__r_int64_t")


def has_get_current_player(module):
    return hasattr(module, "rl_get_current_player__Game_r_int64_t")


def get_num_players(module):
    if has_get_num_players(module):
        return module.functions.get_num_players()
    return 1


def validate_env(module, forced_one_player=False, needs_score=True):
    if not has_get_num_players(module.module) and not has_get_current_player(
        module.module
    ):
        forced_one_player = True
    errors = []
    if not forced_one_player:
        if not has_get_num_players(module.module):
            errors.append(
                '"fun get_num_players() -> Int" is missing, you need to defined it'
            )

        if not has_get_current_player(module.module):
            errors.append(
                '"fun get_current_player(Game game) -> Int" is missing, you need to defined it'
            )

    if not hasattr(module.module, "rl_play__r_Game"):
        errors.append('"fun play() -> Game" is missing, you need to defined it')

    if not has_score(module.module) and needs_score:
        errors.append(
            '"fun score(Game g, Int player_id) -> Float" is missing, you need to defined it, or provide a field score in the action play'
        )

    return errors


class RLCEnvironment(MultiAgentEnv):
    def __init__(
        self, program: Program, dc="", solve_randomness=True, forced_one_player=False
    ):
        self.solve_randomess = solve_randomness
        self.program = program
        self.forced_one_player = forced_one_player
        self.has_score_function = has_score_function(self.module)
        self.has_get_current_player_f = has_get_current_player(self.module)
        if not forced_one_player:
            self.num_agents = get_num_players(self.module)
        else:
            self.num_agents = 1
        self._setup()
        self.metrics_to_log = self._collect_env_metrics_to_log()

    def _setup(self):
        self.state = self.program.start()

        self.state_size = (
            self.program.functions.observation_tensor_size(self.module.Game()) + 1
        )

        self.unwrapper_space = spaces.Dict(
            {
                "observations": spaces.Box(0, 1, shape=(self.state_size,), dtype=int),
                "action_mask": spaces.Box(0, 1, shape=(self.num_actions,), dtype=int),
            }
        )
        self.observation_space = spaces.Dict(
            {i: self.unwrapper_space for i in range(self.num_agents)}
        )
        self.action_space = spaces.Dict(
            {i: spaces.Discrete(self.num_actions) for i in range(self.num_agents)}
        )

        self.spec = Specs(
            5000
            if not has_max_lenght(self.module)
            else self.program.functions.max_game_lenght()
        )

        self._resolve_randomness()
        self.current_score = [self.score(i) for i in range(self.num_agents)]
        self.last_score = self.current_score
        self._agent_ids = [i for i in range(self.num_agents)]
        self._skip_env_checking = True
        super().__init__()
        self._obs_space_in_preferred_format = True
        self._action_space_in_preferred_format = True

    def _collect_env_metrics_to_log(self):
        to_return = {}
        module = self.module
        for name, overloads in module.wrappers.items():
            if not name.startswith("log_"):
                continue

            for overload in overloads:
                if (
                    len(module.signatures[overload]) == 2
                    and (
                        module.signatures[overload][0] == float
                        or module.signatures[overload][0] == int
                    )
                    and module.signatures[overload][1] == module.Game
                ):
                    to_return[name[4:]] = overload

        return to_return

    @property
    def module(self):
        return self.program.module

    @property
    def num_actions(self):
        return self.state.num_actions

    @property
    def current_player(self):
        if self.state.state.resume_index == -1:
            return -4
        if self.forced_one_player:
            return 0
        if not self.has_get_current_player_f:
            return 0
        return self.program.functions.get_current_player(self.state.state)

    @property
    def legal_action_mask(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for i, action in enumerate(self.actions):
            if self.program.functions.can_apply_impl(action, self.state.state).value:
                x.append(1)
            else:
                x.append(0)
        return np.array(x, dtype=np.int8)

    @property
    def actions(self):
        return self.state.actions

    @property
    def legal_actions_indicies(self):
        return self.state.legal_actions_indicies

    @property
    def legal_actions(self):
        return self.state.legal_actions

    def _get_done_winner(self):
        is_done = {
            i: self.state.state.resume_index == -1 for i in range(self.num_agents)
        }
        is_done["__all__"] = self.state.state.resume_index == -1
        scores = {
            i: (self.current_score[i] - self.last_score[i])
            for i in range(self.num_agents)
        }

        return is_done, scores  # return (False, 0.0)

    def score(self, player_id):
        if self.has_score_function:
            return self.program.functions.score(self.state.state, player_id)
        return self.state.state.score

    def log_extra_metrics(self, metrics_logger):
        for name, metric in self.metrics_to_log.items():
            metrics_logger.log_value(name, metric(self.state.state))

    def _get_info(self):
        done, reward = self._get_done_winner()
        return {"reward": reward}

    def _resolve_randomness(self):
        if not self.solve_randomess:
            return
        while self.current_player == -1:  # random player
            action = random.choice(self.state.legal_actions)
            assert self.program.functions.can_apply_impl(action, self.state.state).value
            self.program.functions.apply(action, self.state.state)

    def _current_state(self):

        to_return = {}
        for i in range(self.num_agents):
            serialized = self.module.VectorTdoubleT()
            self.program.functions.resize(serialized, self.state_size)
            self.program.functions.to_observation_tensor(
                self.state.state, i, serialized
            )
            self.program.functions.append(serialized, float(i))

            vec = np.rint(
                np.ctypeslib.as_array(
                    self.program.functions.get(serialized, 0), shape=(self.state_size,)
                )
            ).astype(int)
            to_return[i] = {
                "observations": vec,
                "action_mask": self.legal_action_mask,
            }
        return to_return

    def reset(self, seed=None, options=None, path_to_binary_state=None):
        self.state.reset(
            seed=seed, options=options, path_to_binary_state=path_to_binary_state
        )
        self._resolve_randomness()
        observation = self._current_state()
        info = self._get_info()
        self.current_score = [self.score(i) for i in range(self.num_agents)]
        self.last_score = self.current_score

        return observation, info

    def step(self, action):
        to_apply = (
            action[self.current_player] if self.current_player != -1 else action[0]
        )
        self.state.step(self.actions[to_apply])

        self._resolve_randomness()

        self.last_score = self.current_score
        self.current_score = [self.score(i) for i in range(self.num_agents)]

        done, reward = self._get_done_winner()
        observation = self._current_state()
        info = self._get_info()

        truncated = {i: False for i in range(self.num_agents)}
        truncated["__all__"] = False
        info["current_player"] = self.current_player
        return observation, reward, done, truncated, info

    def _get_next_action_index(self, model, true_self_play):
        if self.current_player == -1:
            return random.choice(self.legal_actions_indicies)
        current_player = self.current_player
        obs = self._current_state()
        policy_id_number = current_player if not true_self_play else 0
        policy_id = f"p{policy_id_number}"
        module = model.get_module(policy_id)
        obs[current_player]["observations"] = torch.tensor(
            np.expand_dims(obs[current_player]["observations"], 0),
            dtype=torch.float32,
        )
        obs[current_player]["action_mask"] = torch.tensor(
            np.expand_dims(obs[current_player]["action_mask"], 0),
            dtype=torch.float32,
        )
        data = {"obs": obs[self.current_player]}
        with torch.no_grad():
            logits = module._forward_inference(data)
            action_probs = torch.softmax(logits["action_dist_inputs"], dim=-1)
            return torch.multinomial(action_probs, num_samples=1)[0, 0].item()

    def one_action_according_to_model(self, model, true_self_play):
        sampled = self._get_next_action_index(model, true_self_play)
        assert self.program.functions.can_apply(
            self.actions[sampled], self.state.state
        ).value
        self.step([sampled for i in range(self.num_agents)])

        return self.actions[sampled]

    def print_probs(self, model, policy_to_use=None):
        obs = self._current_state()
        policy_to_use = policy_to_use if policy_to_use != None else self.current_player
        policy_id = f"p{policy_to_use}"
        module = model.get_module(policy_id)
        obs[self.current_player]["observations"] = torch.tensor(
            np.expand_dims(obs[self.current_player]["observations"], 0),
            dtype=torch.float32,
        )
        obs[self.current_player]["action_mask"] = torch.tensor(
            np.expand_dims(obs[self.current_player]["action_mask"], 0),
            dtype=torch.float32,
        )
        data = {"obs": obs[self.current_player]}
        with torch.no_grad():
            logits = module._forward_inference(data)

            action_probs = torch.softmax(logits["action_dist_inputs"], dim=-1)

            best_actions = [
                pair
                for pair in zip(
                    action_probs[0].tolist(), self.actions, range(self.num_actions)
                )
            ]
            best_actions.sort(key=lambda pair: -pair[0])
            i = 0
            for prob, action, id in best_actions:
                if prob != 0:
                    print(
                        f"{i}: " + self.program.to_string(action),
                        "{:0.4f} %".format(prob * 100),
                    )
                    i = i + 1
            return [id for prob, action, id in best_actions]

    def as_byte_vector(self):
        return self.state.as_byte_vector()

    def from_string(self, string: str) -> bool:
        return self.state.from_string(string)

    def from_byte_vector(self, byte_vector):
        self.state.from_byte_vector(byte_vector)

    def write_binary(self, path: str):
        self.state.write_binary(path)

    def load_binary(self, path: str):
        self.state.load_binary(path)

    def load(self, path: str) -> bool:
        self.state.load(path)
