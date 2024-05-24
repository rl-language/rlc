import ray
import torch
import pickle
import copy
import os
from hyperopt import hp
import gymnasium as gym
import random
import numpy as np
from gymnasium import spaces
from ray.tune.search.hyperopt import HyperOptSearch
from gymnasium.spaces import Dict
from ray.rllib.core.rl_module.rl_module import RLModule, RLModuleConfig

from ray.rllib.env.multi_agent_env import make_multi_agent
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray.rllib.env.multi_agent_env_runner import MultiAgentEnvRunner
from ray.rllib.policy.sample_batch import SampleBatch
from ray.rllib.core.rl_module.marl_module import (
    MultiAgentRLModuleSpec,
    MultiAgentRLModule,
    MultiAgentRLModuleConfig,
)
from ray import train
from ray.rllib.core.rl_module.rl_module import SingleAgentRLModuleSpec
from ray.rllib.env.multi_agent_env import MultiAgentEnv
import wrapper
from ray.rllib.algorithms.ppo.torch.ppo_torch_rl_module import PPOTorchRLModule
from ray.rllib.utils.torch_utils import FLOAT_MIN

from typing import (
    Any,
    Callable,
    Dict,
    KeysView,
    List,
    Mapping,
    Optional,
    Set,
    Type,
    Union,
)



class ActionMaskRLMBase(RLModule):
    def __init__(self, config: RLModuleConfig):
        if not isinstance(config.observation_space, gym.spaces.Dict):
            super().__init__(config)
            return
        # We need to adjust the observation space for this RL Module so that, when
        # building the default models, the RLModule does not "see" the action mask but
        # only the original observation space without the action mask. This tricks it
        # into building models that are compatible with the original observation space.
        config.observation_space = config.observation_space["observations"]

        # The PPORLModule, in its constructor, will build models for the modified
        # observation space.
        super().__init__(config)

def _check_batch(batch):
    """Check whether the batch contains the required keys."""
    if "action_mask" not in batch[SampleBatch.OBS]:
        raise ValueError(
            "Action mask not found in observation. This model requires "
            "the environment to provide observations that include an "
            "action mask (i.e. an observation space of the Dict space "
            "type that looks as follows: \n"
            "{'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
            "'observations': <observation_space>}"
        )
    if "observations" not in batch[SampleBatch.OBS]:
        raise ValueError(
            "Observations not found in observation.This model requires "
            "the environment to provide observations that include a "
            " (i.e. an observation space of the Dict space "
            "type that looks as follows: \n"
            "{'action_mask': Box(0.0, 1.0, shape=(self.action_space.n,)),"
            "'observations': <observation_space>}"
        )


class TorchActionMaskRLM(ActionMaskRLMBase, PPOTorchRLModule):
    def _forward_inference(self, batch, **kwargs):
        return mask_forward_fn_torch(super()._forward_inference, batch, **kwargs)

    def _forward_train(self, batch, *args, **kwargs):
        return mask_forward_fn_torch(super()._forward_train, batch, **kwargs)

    def _forward_exploration(self, batch, *args, **kwargs):
        return mask_forward_fn_torch(super()._forward_exploration, batch, **kwargs)

    def _compute_values(self, batch, device=None):
      _check_batch(batch)

      # Extract the available actions tensor from the observation.
      action_mask = batch[SampleBatch.OBS]["action_mask"]

      # Modify the incoming batch so that the default models can compute logits and
      # values as usual.
      batch[SampleBatch.OBS] = batch[SampleBatch.OBS]["observations"]
      return super()._compute_values(batch, device)


def mask_forward_fn_torch(forward_fn, batch, **kwargs):
    _check_batch(batch)

    # Extract the available actions tensor from the observation.
    action_mask = batch[SampleBatch.OBS]["action_mask"]

    # Modify the incoming batch so that the default models can compute logits and
    # values as usual.
    batch[SampleBatch.OBS] = batch[SampleBatch.OBS]["observations"]

    outputs = forward_fn(batch, **kwargs)

    # Mask logits
    logits = outputs[SampleBatch.ACTION_DIST_INPUTS]
    # Convert action_mask into a [0.0 || -inf]-type mask.
    inf_mask = torch.clamp(torch.log(action_mask), min=FLOAT_MIN)
    masked_logits = logits + inf_mask

    # Replace original values with masked values.
    outputs[SampleBatch.ACTION_DIST_INPUTS] = masked_logits

    return outputs



class Specs:
    def __init__(self, max_steps=100):
        self.max_episode_steps = max_steps
        self.id = random.randint(0, 10)


class TicTacToeRL(MultiAgentEnv):

    def __init__(self, render_mode=None, size=5):
        action = wrapper.AnyGameAction()
        self._actions = wrapper.functions.enumerate(action)
        self.state_size = (
            wrapper.functions.observation_tensor_size(wrapper.Game()).value
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
            {i: self.unwrapper_space for i in range(wrapper.functions.get_num_players().value)}
        )
        self.action_space = spaces.Dict(
            {i: spaces.Discrete(self.num_actions) for i in range(wrapper.functions.get_num_players().value)}
        )

        self.spec = Specs(100)

        self.state = wrapper.functions.play()
        self.current_score = [
            wrapper.functions.score(self.state, i) for i in range(wrapper.functions.get_num_players().value)
        ]
        self.last_score = self.current_score
        self._agent_ids = [i for i in range(wrapper.functions.get_num_players().value)]
        self.num_agents = wrapper.functions.get_num_players().value
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
        is_done = {i: self.state.resume_index == -1 for i in range(self.num_agents)}
        is_done["__all__"] = self.state.resume_index == -1
        scores = {
            i: (self.current_score[i] - self.last_score[i]) for i in range(self.num_agents)
        }

        return is_done, scores         # return (False, 0.0)

    def _get_info(self):
        done, reward = self._get_done_winner()
        return {"reward": reward}

    def reset(self, seed=None, options=None):
        # print("RESET")
        self.state = wrapper.functions.play()
        self.solve_random_choises()
        observation = self._current_state()
        info = self._get_info()
        self.current_score = [
            wrapper.functions.score(self.state, i) for i in range(self.num_agents)
        ]
        self.last_score = self.current_score

        return observation, info

    def solve_random_choises(self):
        while self.current_player() == -1:  # random player
            action = random.choice(self.legal_actions_list())
            # wrapper.functions.print(action)
            assert wrapper.functions.can_apply_impl(action, self.state)
            wrapper.functions.apply(action, self.state)

    def step(self, action):
        to_apply = action[self.current_player()]
        # wrapper.functions.print(self.actions[to_apply])
        if not wrapper.functions.can_apply_impl(self.actions[to_apply], self.state):
            wrapper.functions.apply(random.choice(self.legal_actions_list()), self.state)
        else:
            wrapper.functions.apply(self.actions[to_apply], self.state)

        self.solve_random_choises()

        self.last_score = self.current_score
        self.current_score = [
            wrapper.functions.score(self.state, i) for i in range(self.num_agents)
        ]

        done, reward = self._get_done_winner()
        observation = self._current_state()
        info = self._get_info()

        truncated = {i: False for i in range(self.num_agents)}
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
            for i in range(self.num_agents)
        }


def agent_to_module_mapping_fn(agent_id, episode, **kwargs):
    # agent_id = [0|1] -> module depends on episode ID
    # This way, we make sure that both modules sometimes play agent0
    # (start player) and sometimes agent1 (player to move 2nd).
    modulo_id = hash(episode.id_) % 5
    return "p1"
    assert False

# Step 1: Configure PPO to run 64 parallel workers to collect samples from the env.
ppo_config = (
    PPOConfig()
    .multi_agent(
        policies={"p1"},
        policy_mapping_fn=agent_to_module_mapping_fn,
        policies_to_train=["p1"],
    )
    .experimental(_enable_new_api_stack=True, _disable_preprocessor_api=True)
    .rl_module(
        rl_module_spec=MultiAgentRLModuleSpec(
            module_specs={
                "p1":
                SingleAgentRLModuleSpec(
                    TorchActionMaskRLM, observation_space=TicTacToeRL().unwrapper_space
                ),
            },
       ),
    )
    .training(lr=2e-5)
    .evaluation(evaluation_interval=20, evaluation_parallel_to_training=True, evaluation_config=PPOConfig.overrides(policy_mapping_fn=agent_to_module_mapping_fn), evaluation_num_workers=1)
    .resources(
        num_gpus=1,
        num_gpus_per_learner_worker=1,
        num_cpus_per_worker=1,
        num_cpus_for_local_worker=1,
    )
    .environment(
        env=TicTacToeRL,
        env_config={"observation_space": TicTacToeRL().observation_space},
    )
    .rollouts(
        num_rollout_workers=8, num_envs_per_worker=1, env_runner_cls=MultiAgentEnvRunner
    )
    .framework("torch")
)
ppo_config.num_gpus = 1
ppo_config.model["fcnet_hiddens"] = [512, 512, 512, 512]
ppo_config.model["fcnet_activation"] = "relu"
ppo_config.model["framestack"] = False

stop = {
    "timesteps_total": 1e15,
    # "episode_reward_mean": 2,  # divide by num_agents for actual reward per agent
}


space = {
    "lambda": hp.uniform("lambda", 0.9, 1.0),
    "clip_param": hp.uniform("clip_param", 0.005, 0.02),
    "lr_schedule": [[0, 2e-4], [1e6, 1e-8]],
    "num_sgd_iter": hp.uniformint("num_sgd_iter", 1, 2),
    "sgd_minibatch_size": hp.uniformint("sgd_minibatch_size", 950, 2000),
    "train_batch_size": hp.uniformint("train_batch_size", 2000, 16000),
    "kl_coeff": hp.uniform("kl_coeff", 0.0, 0.3),
    "entropy_coeff": hp.uniform("entropy_coeff", 0.0, 0.3),
}

initial = {
    "lambda": 1.0,
    "clip_param": 0.007,
    "lr_schedule": [[0, 2e-5], [1e6, 1e-8]],
    "num_sgd_iter": 30,
    "sgd_minibatch_size": 128,
    "train_batch_size": 4000,
    #"kl_coeff": 0.23,
    "kl_coeff": 0.00,
    "entropy_coeff": 0.03,
}
ppo_config.lambda_ = 0.94
ppo_config.clip_param = 0.007
ppo_config.lr_schedule = [[0, 2e-5], [1e6, 1e-8]]
ppo_config.num_sgd_iter = 1
ppo_config.sgd_minibatch_size = 2000
ppo_config.train_batch_size = 5000
#ppo_config.kl_coeff = 0.23
ppo_config.kl_coeff = 0.0
ppo_config.entropy_coeff = 0.03

initial2 = {
    "lambda": 0.94,
    "clip_param": 0.007,
    "lr_schedule": [[0, 2e-5], [1e6, 1e-8]],
    "num_sgd_iter": 1,
    "sgd_minibatch_size": 2000,
    "train_batch_size": 5000,
    #"kl_coeff": 0.23,
    "kl_coeff": 0.00,
    "entropy_coeff": 0.03,
}

hyperopt_search = HyperOptSearch(
    space, "episode_reward_mean", mode="max", points_to_evaluate=[initial2]
)

old_run = "."

def train_impl(model, fixed_nets = 0):
    model.learner_group._learner.get_optimizer("p1").param_groups[0]["lr"] = 2e-5
    for _ in range(1000000000):
        for x in range(20):
          for i in range(10):
            print(x, i)
            train.report(model.train())

def load(model):
    #model.restore(f"{old_run}/checkpoint/")
    #model.workers.local_worker().module["p1"].load_state(f"{old_run}/checkpoint/learner/module_state/p1/module_state_dir/")
    #model.workers.local_worker().module["p2"].load_state(f"{old_run}/checkpoint/learner/module_state/p2/module_state_dir/")
    #model.workers.sync_weights(policies=["p1", "p2"])
    with open(f"{old_run}/list_of_fixed.txt", "rb") as f:
      return pickle.load(f)
    return 0

def tuner_run(config):
    config = PPOConfig().update_from_dict(config)
    model = config.build()
    #train_impl(model, load(model))
    train_impl(model, 0)
    model.save()
    model.stop()

# Step 2: Build the PPO algorithm.
# ppo_algo = ppo_config.build()

# print(ppo_algo.evaluate())
# Step 3: Train and evaluate PPO.
# for _ in range(100):
# print(ppo_algo.evaluate())
# ppo_algo.train()

# print(ppo_algo.evaluate())
if __name__ == "__main__":
    from ray import air, tune

    ray.init(num_cpus=12, num_gpus=1)
    #resumption_dir = os.path.abspath("./results")
    resources = PPO.default_resource_request(ppo_config)
    tuner = tune.Tuner(
        tune.with_resources(tuner_run, resources=resources),
        param_space=ppo_config.to_dict(),
        tune_config=ray.tune.TuneConfig(num_samples=1, search_alg=hyperopt_search),
        run_config=air.RunConfig(
            stop=stop,
            verbose=2,
            #storage_path=resumption_dir
        ),
    )

    # if tune.Tuner.can_restore(resumption_dir):
    # tuner = tune.Tuner.restore(os.path.join(resumption_dir, "tuner_run_2024-04-20_13-31-02/"), tune.with_resources(tuner_run, resources=resources))
    results = tuner.fit()

    # best_result = results.get_best_result("episode_reward_mean", mode="max")
    #model = ppo_config.build()
    #model.restore(f"{old_run}/checkpoint/")
    #print(model.evaluate())
    # model = Algorithm.from_checkpoint(best_result.checkpoint.to_directory())
    # model.load_checkpoint(best_result.checkpoint.to_directory())
    # model.load_checkpoint("/home/massimo/ray_results/PPO_2024-04-06_23-43-42/PPO_TicTacToeRL_da25b1fc_1_type=StochasticSampling,disable_action_flattening=False,disable_execution_plan_api=-1,disable_initializ_2024-04-06_23-43-42/checkpoint_000000/")
    # model = Algorithm.from_checkpoint(best_result.checkpoint.to_directory())

    # model.restore("./checkpoint/")
    # model.workers.local_worker().module["p1"].load_state("./checkpoint/learner/module_state/p1/module_state_dir/")
    # model.workers.local_worker().module["p2"].load_state("./checkpoint/learner/module_state/p2/module_state_dir/")
    # if os.path.isfile("./list_of_fixed.txt"):
    #    with open("./list_of_fixed.txt", "rb") as f:
    #        fixed_nets = pickle.load(f)
    # tuner_run(ppo_config)

