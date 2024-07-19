from ray.rllib.env.multi_agent_env import MultiAgentEnv
import torch
import sys
from gymnasium import spaces
from gymnasium.spaces import Dict
import numpy as np
import random

class Specs:
    def __init__(self, max_steps=100):
        self.max_episode_steps = max_steps
        self.id = random.randint(0, 10)


def exit_on_invalid_env(sim, forced_one_player=False):
    errors = validate_env(sim, forced_one_player=forced_one_player)
    if len(errors) == 0:
        return

    for error in errors:
        print(error)
    exit(-1)

def validate_env(wrapper, forced_one_player=False):
    errors = []
    if not forced_one_player:
        if not hasattr(wrapper.module, "rl_get_num_players__r_int64_t"):
            errors.append("\"fun get_num_players() -> Int\" is missing, you need to defined it")

        if not hasattr(wrapper.module, "rl_get_current_player__Game_r_int64_t"):
            errors.append("\"fun get_current_player(Game game) -> Int\" is missing, you need to defined it")

    if not hasattr(wrapper.module, "rl_play__r_Game"):
        errors.append("\"fun play() -> Game\" is missing, you need to defined it")

    if not hasattr(wrapper.module, "rl_score__Game_int64_t_r_double"):
        errors.append("\"fun score(Game g, Int player_id) -> Float\" is missing, you need to defined it")

    if not hasattr(wrapper.module, "rl_max_game_lenght__r_int64_t"):
        errors.append("\"fun max_game_lenght() -> Int\" is missing, you need to defined it")

    return errors

class RLCEnvironment(MultiAgentEnv):
    def __init__(self, wrapper, dc="", solve_randomness=True, forced_one_player=False):
        self.solve_randomess = solve_randomness
        self.wrapper = wrapper
        self.forced_one_player = forced_one_player
        if not forced_one_player:
            self.num_agents = self.wrapper.functions.get_num_players()
        else:
            self.num_agents = 1
        self.setup()

    def setup(self):
        action = self.wrapper.AnyGameAction()
        self._actions = self.wrapper.functions.enumerate(action)
        self.state_size = self.wrapper.functions.observation_tensor_size(self.wrapper.Game()) + 1
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


    @property
    def legal_actions(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for i, action in enumerate(self.actions):
            if self.wrapper.functions.can_apply_impl(action, self.state).value:
                x.append(1)
            else:
                x.append(0)
        return np.array(x, dtype=np.int8)

    def legal_actions_indicies(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for i, action in enumerate(self.actions):
            if self.wrapper.functions.can_apply_impl(action, self.state).value:
                x.append(i)
        return x

    def legal_actions_list(self):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for action in self.actions:
            if self.wrapper.functions.can_apply_impl(action, self.state).value:
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


    def reset(self, seed=None, options=None, path_to_binary_state=None):
        if path_to_binary_state == None:
            self.state = self.wrapper.functions.play()
        else:
            self.load_binary(path_to_binary_state)
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
        if not self.solve_randomess:
            return
        while self.current_player() == -1:  # random player
            action = random.choice(self.legal_actions_list())
            assert self.wrapper.functions.can_apply_impl(action, self.state).value
            self.wrapper.functions.apply(action, self.state)

    def step(self, action):
        to_apply = action[self.current_player()] if self.current_player != -1 else action[0]
        if not self.wrapper.functions.can_apply(
            self.actions[to_apply], self.state
            ).value:
            to_apply = random.choice(self.legal_actions_indicies())
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
        if self.forced_one_player:
            return 0
        return self.wrapper.functions.get_current_player(self.state)

    def _current_state(self):

        to_return = {}
        for i in range(self.num_agents):
          serialized = self.wrapper.VectorTdoubleT()
          self.wrapper.functions.resize(serialized, self.state_size)
          self.wrapper.functions.to_observation_tensor(self.state, i, serialized)
          self.wrapper.functions.append(serialized, float(i))

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

    def _get_next_action_index(self, model):
      if self.current_player() == -1:
          return random.choice(self.legal_actions_indicies())
      obs = self._current_state()
      policy_id = f"p{self.current_player()}"
      module = model.get_module(policy_id)
      obs[self.current_player()]["observations"] = torch.tensor(np.expand_dims(obs[self.current_player()]["observations"], 0), dtype=torch.float32)
      obs[self.current_player()]["action_mask"] = torch.tensor(np.expand_dims(obs[self.current_player()]["action_mask"], 0), dtype=torch.float32)
      data = {"obs": obs[self.current_player()]}
      with torch.no_grad():
        logits = module._forward_inference(data)
        action_probs = torch.softmax(logits["action_dist_inputs"], dim=-1)
        return torch.multinomial(action_probs, num_samples=1)[0, 0].item()

    def one_action_according_to_model(self, model):
        sampled = self._get_next_action_index(model)
        assert self.wrapper.functions.can_apply(self.actions[sampled], self.state).value
        self.step([sampled for i in range(self.num_agents)])

        return self.actions[sampled]

    def print_probs(self, model, policy_to_use=None):
      obs = self._current_state()
      policy_to_use = policy_to_use if policy_to_use != None else self.current_player()
      policy_id = f"p{policy_to_use}"
      module = model.get_module(policy_id)
      obs[self.current_player()]["observations"] = torch.tensor(np.expand_dims(obs[self.current_player()]["observations"], 0), dtype=torch.float32)
      obs[self.current_player()]["action_mask"] = torch.tensor(np.expand_dims(obs[self.current_player()]["action_mask"], 0), dtype=torch.float32)
      data = {"obs": obs[self.current_player()]}
      with torch.no_grad():
        logits = module._forward_inference(data)

        action_probs = torch.softmax(logits["action_dist_inputs"], dim=-1)

        best_actions = [pair for pair in zip(action_probs[0].tolist(), self.actions, range(self.num_actions))]
        best_actions.sort(key=lambda pair: -pair[0])
        i = 0
        for prob, action, id in best_actions:
           if prob != 0:
               print(f"{i}: "+self.action_to_string(action), "{:0.4f} %".format(prob * 100))
               i = i + 1
        return [id for prob, action, id in best_actions]

    def as_byte_vector(self):
        result = self.wrapper.functions.as_byte_vector(self.state)
        real_content = []
        for i in range(getattr(result, "__size")):
            real_content.append(getattr(result, "__data")[i] + 128)
        return bytes(real_content)

    def from_string(self, string: str) -> bool:
        rl_string = self.to_rl_string(str)
        return self.wrapper.functions.from_string(self.state, rl_string)

    def from_byte_vector(self, byte_vector):
        vector = self.wrapper.VectorTint8_tT()
        for byte in byte_vector:
            self.wrapper.functions.append(vector, byte - 128)
        self.wrapper.functions.from_byte_vector(self.state, vector)

    def write_binary(self, path: str):
        with open(path, mode="wb") as file:
            file.write(self.as_byte_vector())
            file.flush()

    def load_binary(self, path: str):
        with open(path, mode="rb") as file:
            bytes = file.read()
            self.from_byte_vector(bytes)

    def load(self, path: str) -> bool:
        with open(path, mode="r") as file:
            bytes = file.read()
            return self.from_string(bytes)

