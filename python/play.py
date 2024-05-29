
import ray
import pickle
import copy
import os
import random

from ml.raylib.environment import RLCEnvironment
from ray.rllib.env.multi_agent_env import make_multi_agent
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray import train
from ml.raylib.module_config import agent_to_module_mapping_fn, get_config

from command_line import load_simulation_from_args, make_rlc_argparse

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


def main():
    parser = make_rlc_argparse("train", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="",
    )
    parser.add_argument("checkpoint", type=str)
    parser.add_argument("--one-agent-per-player", action="store_true", default=True)

    args = parser.parse_args()
    sim, wrapper_path, tmp_dir = load_simulation_from_args(args)

    from ray import air, tune
    num_players = 1 if not args.one_agent_per_player else sim.module.functions.get_num_players().value
    ppo_config, hyperopt_search = get_config(wrapper_path, num_players, exploration=False)
    tune.register_env('rlc_env', lambda config: RLCEnvironment(wrapper_path=wrapper_path, output=args.output))


    ray.init(num_cpus=12, num_gpus=1)
    model = Algorithm.from_checkpoint(args.checkpoint)
    for i in range(num_players):
        model.workers.local_worker().module[f"p{i}"].load_state(f"{args.checkpoint}/learner/module_state/p{i}/module_state_dir/")

    model.evaluate()
    tmp_dir.cleanup()


if __name__ == "__main__":
    main()
