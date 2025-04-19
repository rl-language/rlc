import numpy as np
import ctypes
from gym3 import Env, types
import random
from ml.env import *

from . import tic_tac_toe as ttt


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
        return (
            self.rew[game_index : game_index + 1],
            obs,
            self.first_move[game_index : game_index + 1],
        )

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
