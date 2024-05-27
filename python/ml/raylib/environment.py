from ray.rllib.env.multi_agent_env_runner import MultiAgentEnvRunner
from ray.rllib.core.rl_module.marl_module import (
    MultiAgentRLModuleSpec,
    MultiAgentRLModule,
    MultiAgentRLModuleConfig,
)

class Specs:
    def __init__(self, max_steps=100):
        self.max_episode_steps = max_steps
        self.id = random.randint(0, 10)

def get_env(wrapper):
    class RLCEnvironment(MultiAgentEnv):

        def __init__(self, render_mode=None, size=5):
            action = wrapper.AnyGameAction()
            self._actions = wrapper.functions.enumerate(action)
            self.state_size = (
                wrapper.functions.observation_tensor_size(wrapper.Game()).value * 2
            )
            self.actions = []
            for i in range(wrapper.functions.size(self._actions).value):
                self.actions.append(wrapper.functions.get(self._actions, i).contents)

            self.num_actions = len(self.actions)
            self.unwrapper_space = spaces.Dict(
                {
                    "observations": spaces.Box(0, 1, shape=(self.state_size,), dtype=int),
                    "action_mask": spaces.Box(0, 1, shape=(self.num_actions,), dtype=int),
                }
            )
            self.observation_space = spaces.Dict(
                {i: self.unwrapper_space for i in range(2)}
            )
            self.action_space = spaces.Dict(
                {i: spaces.Discrete(self.num_actions) for i in range(2)}
            )

            self.spec = Specs(10000)

            self.state = wrapper.functions.play()
            self.current_score = [
                wrapper.functions.score(self.state, 0),
                wrapper.functions.score(self.state, 1),
            ]
            self.last_score = self.current_score
            self._agent_ids = [0, 1]
            self.num_agents = 2
            self._skip_env_checking = True
            super().__init__()
            self._obs_space_in_preferred_format = True
            self._action_space_in_preferred_format = True

        @property
        def legal_actions(self):
            # Convert NumPy arrays to nested tuples to make them hashable.
            x = []
            for i, action in enumerate(self.actions):
                if wrapper.functions.can_apply_impl(action, self.state):
                    x.append(1)
                else:
                    x.append(0)
            return np.array(x, dtype=np.int8)

        def legal_actions_list(self):
            # Convert NumPy arrays to nested tuples to make them hashable.
            x = []
            for action in self.actions:
                if wrapper.functions.can_apply_impl(action, self.state):
                    x.append(action)
            return x

        def _get_done_winner(self):
            # if self.state.resume_index == -1:
            # if wrapper.functions.three_in_a_line_player(self.state.board, 1):
            # return (True, 0.0)
            # if wrapper.functions.three_in_a_line_player(self.state.board, 2):
            # return (True, 0.0)
            # return (True, 1.0)
            is_done = {i: self.state.resume_index == -1 for i in range(2)}
            is_done["__all__"] = self.state.resume_index == -1
            scores = {
                i: (self.current_score[i] - self.last_score[i]) for i in range(2)
            }

            return is_done, scores         # return (False, 0.0)

        def _get_info(self):
            done, reward = self._get_done_winner()
            return {"reward": reward}

        def reset(self, seed=None, options=None):
            # print("RESET")
            self.state = wrapper.functions.play()
            observation = self._current_state()
            info = self._get_info()
            self.current_score = [
                wrapper.functions.score(self.state, 0),
                wrapper.functions.score(self.state, 1),
            ]
            self.last_score = self.current_score

            return observation, info

        def step(self, action):
            to_apply = action[self.current_player()]
            # wrapper.functions.print(self.actions[to_apply])
            if not wrapper.functions.can_apply_impl(self.actions[to_apply], self.state):
                wrapper.functions.apply(random.choice(self.legal_actions_list()), self.state)
            else:
                wrapper.functions.apply(self.actions[to_apply], self.state)

            while self.current_player() == -1:  # random player
                action = random.choice(self.legal_actions_list())
                # wrapper.functions.print(action)
                assert wrapper.functions.can_apply_impl(action, self.state)
                wrapper.functions.apply(action, self.state)

            self.last_score = self.current_score
            self.current_score = [
                wrapper.functions.score(self.state, 0),
                wrapper.functions.score(self.state, 1),
            ]

            done, reward = self._get_done_winner()
            observation = self._current_state()
            info = self._get_info()

            truncated = {i: False for i in range(2)}
            truncated["__all__"] = False
            info["current_player"] = self.current_player
            return observation, reward, done, truncated, info

        def current_player(self):
            return wrapper.functions.get_current_player(self.state).value

        def _current_state(self):

            serialized = wrapper.VectorTdoubleT()
            wrapper.functions.resize(serialized, self.state_size)
            # seriazed2 = wrapper.functions.as_byte_vector(self.state)
            wrapper.functions.to_observation_tensor(self.state, serialized)

            vec = np.rint(
                np.ctypeslib.as_array(
                    wrapper.functions.get(serialized, 0), shape=(self.state_size,)
                )
            ).astype(int)
            return {
                i: {
                    "observations": vec,
                    "action_mask": self.legal_actions,
                }
                for i in range(2)
            }

    return RLCEnvironment

