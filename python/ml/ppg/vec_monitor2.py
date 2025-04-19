import time
from collections import deque, namedtuple

import numpy as np
import gym3

Episode = namedtuple(
    "Episode", ["ret", "len", "time", "info", "player_ret", "extra_metrics"]
)


class PostActProcessing(gym3.Wrapper):
    """
    Call process() after each action, except possibly possibly the last
    one which you never called observe for.
    """

    def __init__(self, env):
        super().__init__(env)
        self.need_process = False

    def process_if_needed(self):
        if self.need_process:
            self.process()
            self.need_process = False

    def action_mask(self):
        return self.env.action_mask()

    def current_player(self):
        return self.env.current_player()

    def act(self, ac):
        self.process_if_needed()
        self.env.act(ac)
        self.need_process = True

    def observe(self):
        self.process_if_needed()
        return self.env.observe()

    def process(self):
        raise NotImplementedError

    def get_num_player(self):
        return self.env.get_num_players()


class VecMonitor2(PostActProcessing):
    def __init__(
        self,
        venv: "(gym3.Env)",
        keep_buf: "(int) how many returns/lengths/infos to keep" = 0,
        keep_sep_eps: "keep separate buffer per env" = False,
        keep_non_rolling: "keep separate buffer that must be explicitly cleared" = False,
    ):
        """
        use n_per_env if you want to keep sep
        """
        super().__init__(venv)
        self.eprets = None
        self.eplens = None
        self.epcount = 0
        self.tstart = time.time()
        if keep_buf:
            self.ep_buf = deque([], maxlen=keep_buf)
        else:
            self.ep_buf = None
        if keep_sep_eps:
            self.per_env_buf = [[] for _ in range(self.num)]
        else:
            self.per_env_buf = None
        if keep_non_rolling:
            self.non_rolling_buf = deque([])
        else:
            self.non_rolling_buf = None
        self.eprets = np.zeros(self.num, "f")
        self.eprets_player = np.zeros((self.num, venv.get_num_players()), "f")
        self.eplens = np.zeros(self.num, "i")

    def process(self):
        players_to_act = self.env.previous_players()
        lastrews, _obs, firsts = self.env.observe()
        infos = self.env.get_info()
        self.eprets += lastrews
        for i, player in enumerate(players_to_act):
            self.eprets_player[i, player] += lastrews[i]
        self.eplens += 1
        for i in range(self.num):
            if self.env.first_for_all_players(i):
                timefromstart = round(time.time() - self.tstart, 6)
                ep = Episode(
                    self.eprets[i],
                    self.eplens[i],
                    timefromstart,
                    infos[i],
                    np.copy(self.eprets_player[i]),
                    np.copy(self.env.get_previous_episode_extra_metrics(i)),
                )
                if self.ep_buf is not None:
                    self.ep_buf.append(ep)
                if self.per_env_buf is not None:
                    self.per_env_buf[i].append(ep)
                if self.non_rolling_buf is not None:
                    self.non_rolling_buf.append(ep)
                self.epcount += 1
                self.eprets[i] = 0
                self.eplens[i] = 0
                self.eprets_player[i] = np.zeros(self.env.get_num_players(), "f")

    def clear_episode_bufs(self):
        if self.ep_buf:
            self.ep_buf.clear()
        self.clear_per_env_episode_buf()
        self.clear_non_rolling_episode_buf()

    def clear_per_env_episode_buf(self):
        if self.per_env_buf:
            for i in range(self.num):
                self.per_env_buf[i].clear()

    def clear_non_rolling_episode_buf(self):
        if self.non_rolling_buf:
            self.non_rolling_buf.clear()

    def log_extra_metrics(self, metric):
        return self.env.log_extra_metrics(metric)

    def step_one(self, i, action):
        return self.env.step_one(i, action)

    def current_player_one(self, index):
        return self.env.current_player_one(index)

    def one_action_mask(self, game_id):
        return self.env.one_action_mask(game_id)

    def observe_one(self, i):
        return self.env.observe_one(i)

    def get_user_defined_log_functions(self):
        return self.env.get_user_defined_log_functions()
