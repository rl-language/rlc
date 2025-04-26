try:
    import numpy
except:
    print("Failed to import numpy, install it with python -m pip install numpy , or a equivalent command")
    exit(-1)

import numpy as np
import ctypes
import random


def exit_on_invalid_env(program, forced_one_player=False, needs_score=True):
    errors = validate_env(
        program, forced_one_player=forced_one_player, needs_score=needs_score
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
        return module.get_num_players()
    return 1


def validate_env(program, forced_one_player=False, needs_score=True):
    program.module.emit_observation_tensor_warnings(program.module.play())
    if not has_get_num_players(program.module) and not has_get_current_player(
        program.module
    ):
        forced_one_player = True
    errors = []
    if not program.module.print_enumeration_errors(program.module.AnyGameAction()):
        errors.append("Invalid actions")
    if not forced_one_player:
        if not has_get_num_players(program.module):
            errors.append(
                '"fun get_num_players() -> Int" is missing, you need to defined it'
            )

        if not has_get_current_player(program.module):
            errors.append(
                '"fun get_current_player(Game game) -> Int" is missing, you need to defined it'
            )

    if not hasattr(program.module, "rl_play__r_Game"):
        errors.append('"fun play() -> Game" is missing, you need to defined it')

    if not has_score(program.module) and needs_score:
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
        self.valid_action_vector = self.module.make_valid_actions_vector(
            self.state.raw_actions, self.state.state
        )
        self.score_fn = (
            (lambda g, p: self.state.state.score)
            if not self.has_score_function
            else self.module.rl_score__Game_int64_t_r_double
        )
        self.num_actions = len(self.state.actions)
        self.serialized = self.module.VectorTdoubleT()
        self.serialized.resize(self.get_state_size())
        self.serialized_player_id = self.serialized.get(self.get_state_size() - 1)
        self.to_observation_tensor = (
            self.module.rl_to_observation_tensor__Game_int64_t_VectorTdoubleT
        )
        self.get_valid_actions = (
            self.module.rl_get_valid_actions__VectorTint8_tT_VectorTAnyGameActionT_Game
        )
        self.ptr_to_valid_actions = self.valid_action_vector.get(0)
        self.last_valid_action_mask = np.ctypeslib.as_array(
            self.ptr_to_valid_actions, shape=(self.num_actions,)
        )

        self.rng = np.random.default_rng()
        self.random_numbers = self.rng.integers(
            low=0, high=self.num_actions, size=1_000_000
        )
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

    def can_apply(self, action) -> bool:
        return self.module.can_apply_impl(action, self.state.state)

    def _resolve_randomness(self):
        if not self.solve_randomess:
            return
        while self.get_current_player() == -1:  # random player
            action_index = self.random_valid_action_index()
            action = self.actions()[action_index]
            assert self.module.can_apply_impl(action, self.state.state)
            self.module.apply(action, self.state.state)

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
        self.random_numbers = self.rng.integers(
            low=0, high=self.num_actions, size=1_000_000
        )

    def is_first_move(self, player_id):
        return self.first_move[player_id]

    def step(self, action):
        return self.step_action(self.actions()[action])

    def step_action(self, action):
        if self.is_terminating_episode:
            current_player = self.players_final_turn[0]
            self.last_score[current_player] = self.current_score[current_player]
            self.current_score[current_player] = self.total_score(current_player)
            self.players_final_turn.pop(0)
            return self.step_score(current_player)
        current_player = self.get_current_player()
        self.first_move[current_player] = False
        self.last_score[current_player] = self.current_score[current_player]
        self.state.step(action)
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
        return self.module.observation_tensor_size(self.module.Game()) + 1

    def get_state(self):
        self.to_observation_tensor(self.state.state, 0, self.serialized)
        self.serialized_player_id.contents = ctypes.c_double(0)

        return np.ctypeslib.as_array(
            self.serialized.get(0),
            shape=(self.get_state_size(), 1, 1),
        ).copy()

    def is_done_underling(self):
        return self.state.state.resume_index == -1

    def pretty_print(self):
        self.state.pretty_print()

    def print(self):
        self.state.print()

    def is_done_for_everyone(self):
        return self.state.state.resume_index == -1 and len(self.players_final_turn) == 0
