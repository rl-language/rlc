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

from raylib.action_mask import ActionMask
from ray.rllib.env.multi_agent_env import make_multi_agent
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray.rllib.env.multi_agent_env_runner import MultiAgentEnvRunner
from ray.rllib.core.rl_module.marl_module import (
    MultiAgentRLModuleSpec,
    MultiAgentRLModule,
    MultiAgentRLModuleConfig,
)
from ray import train
from ray.rllib.core.rl_module.rl_module import SingleAgentRLModuleSpec
from ray.rllib.env.multi_agent_env import MultiAgentEnv
import wrapper
from ray.rllib.utils.torch_utils import FLOAT_MIN
from raylib.environment import RLCEnvironment
from raylib.module_config import agent_to_module_mapping_fn, get_config

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


old_run = "/tmp/ray/session_2024-04-30_15-29-12_620166_946517/artifacts/2024-04-30_15-29-14/tuner_run_2024-04-30_15-29-14/working_dirs/tuner_run_TicTacToeRL_9fc95a60_1_type=StochasticSampling,disable_action_flattening=False,disable_execution_plan_api=-1,disable_ini_2024-04-30_15-29-14/"
old_run = "."

def train_impl(model, fixed_nets = 0):
    model.learner_group._learner.get_optimizer("p2").param_groups[0]["lr"] = 2e-5
    model.learner_group._learner.get_optimizer("p1").param_groups[0]["lr"] = 2e-5
    for _ in range(1000000000):
        with open(f"{old_run}/list_of_fixed.txt", "wb+") as f:
          if not os.path.exists(f"{old_run}/net_p3_{fixed_nets}"):
            os.makedirs(f"{old_run}/net_p3_{fixed_nets}")
          if not os.path.exists(f"f{old_run}/net_p4_{fixed_nets}"):
            os.makedirs(f"{old_run}/net_p4_{fixed_nets}")
          model.workers.local_worker().module["p1"].save_state(f"{old_run}/net_p3_{fixed_nets}")
          model.workers.local_worker().module["p2"].save_state(f"{old_run}/net_p4_{fixed_nets}")
          fixed_nets = fixed_nets + 1
          pickle.dump(fixed_nets, f)
        for x in range(20):
          for i in range(10):
            print(x, i)
            train.report(model.train())
            if fixed_nets != 0:
              model.workers.local_worker().module["p3"].load_state(f"{old_run}/net_p3_{random.choice(range(fixed_nets))}")
              model.workers.local_worker().module["p4"].load_state(f"{old_run}/net_p4_{random.choice(range(fixed_nets))}")
              model.workers.sync_weights(policies=["p3", "p4"])
        model.save(f"{old_run}/checkpoint")

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
    ppo_config, hyperopt_search = get_config()

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

