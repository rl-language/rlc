import numpy as np
import ctypes
from gym3 import Env, types, Wrapper
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
        self.num_players = get_num_players(self.module)
        self.state = self.program.start()
        self.valid_action_vector = program.module.functions.make_valid_actions_vector(
            self.state.raw_actions, self.state.state
        )
        self.score_fn = (
            (lambda g, p: self.state.state.score)
            if not self.has_score_function
            else self.module.rl_score__Game_int64_t_r_double
        )
        self.num_actions = len(self.state.actions)
        self.serialized = self.module.VectorTdoubleT()
        self.program.functions.resize(self.serialized, self.get_state_size())
        self.serialized_player_id = self.program.functions.get(
            self.serialized, self.get_state_size() - 1
        )
        self.to_observation_tensor = (
            self.module.rl_to_observation_tensor__Game_int64_t_VectorTdoubleT
        )
        self.get_valid_actions = (
            self.module.rl_get_valid_actions__VectorTint8_tT_VectorTAnyGameActionT_Game
        )
        self.ptr_to_valid_actions = self.program.functions.get(
            self.valid_action_vector, 0
        )
        self.last_valid_action_mask = np.ctypeslib.as_array(
            self.ptr_to_valid_actions, shape=(self.num_actions,)
        )

        self.rng = np.random.default_rng()
        self.random_numbers = self.rng.integers(low=0, high=self.num_actions, size=1_000_000)
        self.current_random_index = 0

        self.current_player_fn = (
            (lambda state: 0)
            if not self.has_get_current_player_f
            else self.module.rl_get_current_player__Game_r_int64_t
        )

        self.is_terminating_episode = False
        self.players_final_turn = []

        self._resolve_randomness()
        self.last_score = [0.0 for p in range(self.num_players)]
        self.current_score = [0.0 for p in range(self.num_players)]
        self.first_move = [True for p in range(self.num_players)]

        self.user_defined_log_functions = self._collect_env_metrics_to_log()

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

    def log_extra_metrics(self, metric):
        value = metric(self.state.state)
        if isinstance(value, int):
            return value
        else:
            return value.value

    def legal_actions(self):
        return self.state.legal_actions

    def actions(self):
        return self.state.actions

    def get_current_player(self):
        if self.is_terminating_episode:
            return self.players_final_turn[0]
        if self.state.state.resume_index == -1:
            return -4
        return self.current_player_fn(self.state.state)

    def random_valid_action_index(self):
        arr = self.get_action_mask()
        ones_indicies = np.where(arr == 1)[0]
        to_return = ones_indicies[
            self.random_numbers[self.current_random_index] % len(ones_indicies)
        ]
        self.current_random_index = (self.current_random_index + 1) % 1_000_000
        return to_return

    def get_action_mask(self):
        self.get_valid_actions(
            self.valid_action_vector, self.state.raw_actions, self.state.state
        )
        return self.last_valid_action_mask

    def total_score(self, player_id):
        score = self.score_fn(self.state.state, player_id)
        if isinstance(score, float):
            return score
        return score.value

    def step_score(self, player_id):
        return self.current_score[player_id] - self.last_score[player_id]

    def _resolve_randomness(self):
        if not self.solve_randomess:
            return
        while self.get_current_player() == -1:  # random player
            action_index = self.random_valid_action_index()
            action = self.actions()[action_index]
            assert self.program.functions.can_apply_impl(action, self.state.state).value
            self.program.functions.apply(action, self.state.state)

    def reset(self, seed=None, options=None, path_to_binary_state=None):
        self.is_terminating_episode = False
        self.players_final_turn = []
        self.state.reset(
            seed=seed, options=options, path_to_binary_state=path_to_binary_state
        )
        self._resolve_randomness()
        self.last_score = [0.0 for p in range(self.num_players)]
        self.current_score = [0.0 for p in range(self.num_players)]
        self.first_move = [True for p in range(self.num_players)]
        self.random_numbers = self.rng.integers(low=0, high=self.num_actions, size=1_000_000)

    def is_first_move(self, player_id):
        return self.first_move[player_id]

    def step(self, action):
        if self.is_terminating_episode:
            current_player = self.players_final_turn[0]
            self.last_score[current_player] = self.current_score[current_player]
            self.current_score[current_player] = self.total_score(current_player)
            self.players_final_turn.pop(0)
            return self.step_score(current_player)
        current_player = self.get_current_player()
        self.first_move[current_player] = False
        self.last_score[current_player] = self.current_score[current_player]
        self.state.step(self.actions()[action])
        self._resolve_randomness()
        self.current_score[current_player] = self.total_score(current_player)
        if self.is_done_underling():
            self.players_final_turn = [
                i for i in range(self.num_players) if i != current_player
            ]
            self.is_terminating_episode = True
        return self.step_score(current_player)

    def get_action_count(self):
        return self.num_actions

    def get_state_size(self):
        return self.program.functions.observation_tensor_size(self.module.Game()) + 1

    def get_state(self):
        self.to_observation_tensor(self.state.state, 0, self.serialized)
        self.serialized_player_id.contents = ctypes.c_double(0)

        vec = np.rint(
            np.ctypeslib.as_array(
                self.program.functions.get(self.serialized, 0),
                shape=(self.get_state_size(), 1, 1),
            )
        ).astype(float)
        return vec

    def is_done_underling(self):
        return self.state.state.resume_index == -1

    def pretty_print(self):
        self.state.pretty_print()

    def print(self):
        self.state.print()

    def is_done_for_everyone(self):
        return self.state.state.resume_index == -1 and len(self.players_final_turn) == 0


class RLCMultiEnv(Env):
    def __init__(self, program, num=1, seed=None, solve_randomess=True):
        self.num = num
        self.games = [
            SingleRLCEnvironment(program, solve_randomess) for i in range(num)
        ]

        self.ob_space = types.TensorType(
            types.Real(), shape=(self.games[0].get_state_size(), 1, 1)
        )
        self.ac_space = types.TensorType(
            types.Discrete(n=self.games[0].get_action_count()), shape=(1,)
        )

        self.first_for_all = np.ones(self.num, dtype=bool)
        self.first_move = np.ones(self.num, dtype=bool)
        self.rew = np.zeros(self.num, dtype=np.float32)
        self.just_acted_players = self.current_player()
        self.previous_game_custom_log_metrics = np.zeros(
            (self.num, len(self.get_user_defined_log_functions())), dtype=np.float32
        )
        self.num_actions = self.games[0].num_actions

        super().__init__(ob_space=self.ob_space, ac_space=self.ac_space, num=self.num)

    def get_num_players(self):
        return self.games[0].num_players

    def action_mask(self):
        return np.array([g.get_action_mask() for g in self.games])

    def one_action_mask(self, game_id):
        return np.array([self.games[game_id].get_action_mask()])

    def current_player(self):
        return np.array([g.get_current_player() for g in self.games])

    def current_player_one(self, index):
        return self.games[index].get_current_player()

    def previous_players(self):
        return self.just_acted_players

    def observe(self):
        obs = np.array([g.get_state() for g in self.games])
        return self.rew, obs, self.first_move

    def observe_one(self, game_index):
        obs = np.array([self.games[game_index].get_state()])
        return self.rew[game_index:game_index+1], obs, self.first_move[game_index:game_index+1]

    def act(self, ac):
        self.step(ac)

    def is_done_for_everyone(self, game_id):
        return self.games[game_id].is_done_for_everyone()

    def step_one(self, i, action):
        game = self.games[i]
        self.just_acted_players[i] = game.get_current_player()
        self.rew[i] = game.step(action[0])
        self.first_for_all[i] = False
        if game.is_done_for_everyone():
            for metric_id, metric in enumerate(
                self.get_user_defined_log_functions().values()
            ):
                self.previous_game_custom_log_metrics[i, metric_id] = (
                    game.log_extra_metrics(metric)
                )
            game.reset()
            self.first_for_all[i] = True
        self.first_move[i] = game.is_first_move(game.get_current_player())

    def step(self, ac):
        for i, action in enumerate(ac):
            self.step_one(i, action)

        return self.observe()

    def first_for_all_players(self, game_id):
        return self.first_for_all[game_id]

    def pretty_print(self, game_id):
        self.games[game_id].pretty_print()

    def print(self, game_id):
        self.games[game_id].print()

    def get_user_defined_log_functions(self):
        return self.games[0].user_defined_log_functions

    def log_extra_metrics(self, game_id, metric):
        return self.games[game_id].log_extra_metrics(metric)

    def get_previous_episode_extra_metrics(self, game_id):
        return self.previous_game_custom_log_metrics[game_id, :]
