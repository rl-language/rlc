from ray.rllib.env.multi_agent_env_runner import MultiAgentEnvRunner
from ray.rllib.env.multi_agent_env import MultiAgentEnv
import torch
import sys
from gymnasium import spaces
from gymnasium.spaces import Dict
import numpy as np
from importlib import import_module, machinery, util
import random
from ray.rllib.core.rl_module.marl_module import (
    MultiAgentRLModuleSpec,
    MultiAgentRLModule,
    MultiAgentRLModuleConfig,
)


class Specs:
    def __init__(self, max_steps=100):
        self.max_episode_steps = max_steps
        self.id = random.randint(0, 10)


def import_file(name, file_path):
    loader = machinery.SourceFileLoader(name, file_path)
    spec = util.spec_from_loader(name, loader)
    mod = util.module_from_spec(spec)
    loader.exec_module(mod)
    return mod


class RLCEnvironment(MultiAgentEnv):
    def __init__(self, dc="", wrapper_path="", output=None):
        self.wrapper_path = wrapper_path
        self.output_path = output
        self.output = None
        self.setup()

    def setup(self):
        self.reset_output()
        self.wrapper = import_file("wrapper", self.wrapper_path)
        action = self.wrapper.AnyGameAction()
        self.num_agents = self.wrapper.functions.get_num_players()
        self._actions = self.wrapper.functions.enumerate(action)
        self.state_size = self.wrapper.functions.observation_tensor_size(self.wrapper.Game())
        self.actions = []
        for i in range(self.wrapper.functions.size(self._actions)):
            self.actions.append(self.wrapper.functions.get(self._actions, i).contents)

        self.num_actions = len(self.actions)
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

        self.spec = Specs(self.wrapper.functions.max_game_lenght())

        self.state = self.wrapper.functions.play()
        self.resolve_randomness()
        self.current_score = [
            self.wrapper.functions.score(self.state, i) for i in range(self.num_agents)
        ]
        self.last_score = self.current_score
        self._agent_ids = [i for i in range(self.num_agents)]
        self._skip_env_checking = True
        super().__init__()
        self._obs_space_in_preferred_format = True
        self._action_space_in_preferred_format = True

    def __getstate__(self):
        return {
            "wrapper_path": self.wrapper_path,
        }

    def __setstate__(self, state):
        self.wrapper_path = state["wrapper_path"]
        self.setup()

    @property
    def legal_actions(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for i, action in enumerate(self.actions):
            if self.wrapper.functions.can_apply_impl(action, self.state):
                x.append(1)
            else:
                x.append(0)
        return np.array(x, dtype=np.int8)

    def legal_actions_indicies(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for i, action in enumerate(self.actions):
            if self.wrapper.functions.can_apply_impl(action, self.state):
                x.append(i)
        return x

    def legal_actions_list(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for action in self.actions:
            if self.wrapper.functions.can_apply_impl(action, self.state):
                x.append(action)
        return x

    def _get_done_winner(self):
        is_done = {i: self.state.resume_index == -1 for i in range(self.num_agents)}
        is_done["__all__"] = self.state.resume_index == -1
        scores = {
            i: (self.current_score[i] - self.last_score[i])
            for i in range(self.num_agents)
        }

        return is_done, scores  # return (False, 0.0)

    def _get_info(self):
        done, reward = self._get_done_winner()
        return {"reward": reward}

    def reset_output(self):
        if self.output_path is None:
            pass
        elif self.output_path == "":
            self.output = sys.stdout
        else:
            if self.output != None:
                self.output.close()
            self.output = open(self.output_path, "w+")

    def reset(self, seed=None, options=None):
        self.reset_output()
        self.state = self.wrapper.functions.play()
        self.resolve_randomness()
        observation = self._current_state()
        info = self._get_info()
        self.current_score = [
            self.wrapper.functions.score(self.state, i) for i in range(self.num_agents)
        ]
        self.last_score = self.current_score

        return observation, info

    def action_to_string(self, action):
        return self.to_python_string(self.wrapper.functions.to_string(action))

    def to_rl_string(self, string):
        return self.wrapper.rl_s__strlit_r_String(string)

    def to_python_string(self, string):
        first_character = getattr(getattr(string, "__data"), "__data")
        return self.wrapper.cast(first_character, self.wrapper.c_char_p).value.decode(
            "utf-8"
        )

    def resolve_randomness(self):
        while self.current_player() == -1:  # random player
            action = random.choice(self.legal_actions_list())
            if self.output is not None:
                self.output.write(
                    self.to_python_string(self.wrapper.functions.to_string(action))
                )
                self.output.write("\n")
            assert self.wrapper.functions.can_apply_impl(action, self.state)
            self.wrapper.functions.apply(action, self.state)

    def step(self, action):
        to_apply = action[self.current_player()]
        if self.output is not None:
            act = self.actions[to_apply]
            self.output.write(
                self.to_python_string(self.wrapper.functions.to_string(act))
            )
            self.output.write("\n")
            self.output.flush()
        if not self.wrapper.functions.can_apply_impl(
            self.actions[to_apply], self.state
        ):
            self.wrapper.functions.apply(
                random.choice(self.legal_actions_list()), self.state
            )
        else:
            self.wrapper.functions.apply(self.actions[to_apply], self.state)

        self.resolve_randomness()

        self.last_score = self.current_score
        self.current_score = [
            self.wrapper.functions.score(self.state, i) for i in range(self.num_agents)
        ]

        done, reward = self._get_done_winner()
        observation = self._current_state()
        info = self._get_info()

        truncated = {i: False for i in range(self.num_agents)}
        truncated["__all__"] = False
        info["current_player"] = self.current_player
        return observation, reward, done, truncated, info

    def current_player(self):
        return self.wrapper.functions.get_current_player(self.state)

    def _current_state(self):

        to_return = {}
        for i in range(self.num_agents):
          serialized = self.wrapper.VectorTdoubleT()
          self.wrapper.functions.resize(serialized, self.state_size)
          self.wrapper.functions.to_observation_tensor(self.state, i, serialized)

          vec = np.rint(
            np.ctypeslib.as_array(
                self.wrapper.functions.get(serialized, 0), shape=(self.state_size,)
            )
          ).astype(int)
          to_return[i] = {
                "observations": vec,
                "action_mask": self.legal_actions,
          }
        return to_return

    def print_probs(self, model):
      obs = self._current_state()
      policy_id = f"p{self.current_player()}"
      module = model.get_module(policy_id)
      obs[self.current_player()]["observations"] = torch.tensor(np.expand_dims(obs[self.current_player()]["observations"], 0), dtype=torch.float32)
      obs[self.current_player()]["action_mask"] = torch.tensor(np.expand_dims(obs[self.current_player()]["action_mask"], 0), dtype=torch.float32)
      data = {"obs": obs[self.current_player()]}
      with torch.no_grad():
        logits = module._forward_inference(data)

        action_probs = torch.softmax(logits["action_dist_inputs"], dim=-1)

        best_actions = [pair for pair in zip(action_probs[0].tolist(), self.actions, range(self.num_actions))]
        best_actions.sort(key=lambda pair: -pair[0])
        for prob, action, id in best_actions:
           if prob != 0:
               print(self.action_to_string(action), "{:0.4f} %".format(prob * 100))
        return best_actions[0][2]
