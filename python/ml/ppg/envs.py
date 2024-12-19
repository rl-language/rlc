import numpy as np
import ctypes
from gym3 import Env, types, Wrapper
from procgen import ProcgenGym3Env
import random

from . import tic_tac_toe as ttt

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


class SingleRLCEnvironment:
    def __init__(self, program, solve_randomness=True):
        self.program = program
        self.module = self.program.module
        self.solve_randomess = solve_randomness
        self.has_score_function = has_score_function(self.module)
        self.has_get_current_player_f = has_get_current_player(self.module)
        self.num_players = get_num_players(self.program)
        self.state = self.program.start()
        self.valid_action_vector = program.module.functions.make_valid_actions_vector(self.state.raw_actions, self.state.state)
        self.score_fn = (lambda g, p: self.state.state.score) if not self.has_score_function else self.module.rl_score__Game_int64_t_r_double
        self.num_actions = len(self.state.actions)
        self.serialized = self.module.VectorTdoubleT()
        self.program.functions.resize(self.serialized, self.get_state_size())
        self.serialized_player_id = self.program.functions.get(self.serialized, self.get_state_size() - 1)
        self.to_observation_tensor = self.module.rl_to_observation_tensor__Game_int64_t_VectorTdoubleT
        self.get_valid_actions = self.module.rl_get_valid_actions__VectorTint8_tT_VectorTAnyGameActionT_Game
        self.ptr_to_valid_actions = self.program.functions.get(self.valid_action_vector, 0)
        self.last_valid_action_mask = np.ctypeslib.as_array(self.ptr_to_valid_actions, shape=(self.num_actions, ))

        rng = np.random.default_rng()
        self.random_numbers = rng.integers(low=0, high=self.num_actions, size=1_000_000)
        self.current_random_index = 0

        self.current_player_fn = (lambda state: 0) if not self.has_get_current_player_f else self.module.rl_get_current_player__Game_r_int64_t

        self._resolve_randomness()
        self.last_score = 0.0
        self.current_score = 0.0


    def legal_actions(self):
        return self.state.legal_actions

    def actions(self):
        return self.state.actions

    def get_current_player(self):
        if self.state.state.resume_index == -1:
            return -4
        return self.current_player_fn(self.state.state)

    def random_valid_action_index(self):
        arr = self.get_action_mask()
        ones_indicies = np.where(arr == 1)[0]
        to_return = ones_indicies[self.random_numbers[self.current_random_index] % len(ones_indicies)]
        self.current_random_index = self.current_random_index % 100_000_000
        return to_return

    def get_action_mask(self):
        self.get_valid_actions(self.valid_action_vector, self.state.raw_actions, self.state.state)
        return self.last_valid_action_mask

    def total_score(self, player_id):
        return self.score_fn(self.state.state, player_id).value

    def step_score(self, player_id):
        return self.current_score - self.last_score

    def _resolve_randomness(self):
        if not self.solve_randomess:
            return
        while self.get_current_player() == -1:  # random player
            action_index = self.random_valid_action_index()
            action = self.actions()[action_index]
            assert self.program.functions.can_apply_impl(action, self.state.state).value
            self.program.functions.apply(action, self.state.state)

    def reset(self, seed=None, options=None, path_to_binary_state=None):
        self.state.reset(
            seed=seed, options=options, path_to_binary_state=path_to_binary_state
        )
        self._resolve_randomness()
        self.last_score = 0.0
        self.current_score = 0.0

    def step(self, action):
        self.last_score = self.current_score
        self.state.step(self.actions()[action])
        self._resolve_randomness()
        self.current_score = self.total_score(0)

    def get_action_count(self):
        return self.num_actions

    def get_state_size(self):
        return self.program.functions.observation_tensor_size(self.module.Game()) + 1

    def get_state(self):
        self.to_observation_tensor(
            self.state.state, 0, self.serialized
        )
        self.serialized_player_id.contents = ctypes.c_double(0)

        vec = np.rint(
            np.ctypeslib.as_array(
                self.program.functions.get(self.serialized, 0), shape=(self.get_state_size(), 1, 1)
            )
        ).astype(float)
        return vec

    def is_done(self):
        return self.state.state.resume_index == -1

class RLCMultiEnv(Env):
    def __init__(self, program, num=1, seed=None, solve_randomess=True):
        self.num = num
        self.games = [SingleRLCEnvironment(program, solve_randomess) for i in range(num)]

        self.ob_space = types.TensorType(types.Real(), shape=(self.games[0].get_state_size(), 1, 1))
        self.ac_space = types.TensorType(types.Discrete(n=self.games[0].get_action_count()), shape=(1,))

        self.done = np.ones(self.num, dtype=bool)
        self.rew = np.zeros(self.num, dtype=np.float32)

        super().__init__(
            ob_space=self.ob_space,
            ac_space=self.ac_space,
            num=self.num
        )

    def action_mask(self):
        return np.array([g.get_action_mask() for g in self.games])

    def observe(self):
        obs = np.array([g.get_state() for g in self.games])
        return self.rew, obs, self.done

    def act(self, ac):
        self.step(ac)

    def step(self, ac):
        for i, action in enumerate(ac):
            game = self.games[i]
            game.step(action[0])
            self.rew[i] = game.step_score(i)
            self.done[i] = game.is_done()
            if game.is_done():
                game.reset()

        return self.observe()

def get_procgen_venv(*, env_id, num_envs, rendering=False, **env_kwargs):
    if rendering:
        env_kwargs["render_human"] = True

    if env_id == "tic_tac_toe":
        return ttt.TicTacToeEnv(num_envs)

    env = ProcgenGym3Env(num=num_envs, env_name=env_id, **env_kwargs)

    env = gym3.ExtractDictObWrapper(env, "rgb")

    if rendering:
        env = gym3.ViewerWrapper(env, info_key="rgb")
    return env

def get_venv(num_envs, env_name, **env_kwargs):
    venv = get_procgen_venv(num_envs=num_envs, env_id=env_name, **env_kwargs)

    return venv
