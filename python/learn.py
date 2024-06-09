import ray
import pickle
import copy
import math
import os
import random

from ml.raylib.environment import RLCEnvironment
from ray.rllib.env.multi_agent_env import make_multi_agent
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray import train
from ml.raylib.module_config import get_config

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


def get_multi_train(num_players):
    def train_impl(config, fixed_nets=0):
        config = PPOConfig().update_from_dict(config)
        model = config.build()

        for _ in range(1000000000):
            with open(f"./list_of_fixed.txt", "wb+") as f:
                for i in range(num_players):
                    if not os.path.exists(f"./net_p{num_players + i}_{fixed_nets}"):
                        os.makedirs(f"./net_p{num_players + i}_{fixed_nets}")
                    model.workers.local_worker().module[f"p{i}"].save_state(
                        f"./net_p{num_players + i}_{fixed_nets}"
                    )
                fixed_nets = fixed_nets + 1
                pickle.dump(fixed_nets, f)
            for x in range(20):
                for i in range(10):
                    print(x, i)
                    train.report(model.train())
                    if fixed_nets != 0:
                        for i in range(num_players):
                            model.workers.local_worker().module[
                                f"p{i + num_players}"
                            ].load_state(
                                f"./net_p{i + num_players}_{random.choice(range(fixed_nets))}"
                            )
                        model.workers.sync_weights(
                            policies=[f"p{i + num_players}" for i in range(num_players)]
                        )
            model.save(f"./checkpoint")
        model.save()
        model.stop()

    return train_impl


def get_trainer(output_path, total_train_iterations):
    def single_agent_train(config):
        config = PPOConfig().update_from_dict(config)
        model = config.build()
        for _ in range(math.ceil(total_train_iterations / 10)):
            for i in range(10):
                train.report(model.train())
            if output_path != "":
                model.save(output_path)
        model.save()
        model.stop()

    return single_agent_train


def main():
    parser = make_rlc_argparse("train", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="./network",
    )
    parser.add_argument("--true-self-play", action="store_true", default=False)
    parser.add_argument("--league-play", action="store_true", default=False)
    parser.add_argument("--total-train-iterations", default=100000000, type=int)
    parser.add_argument("--sample-space", default=1, type=int)

    args = parser.parse_args()
    ray.init(num_cpus=12, num_gpus=1, include_dashboard=False)
    with load_simulation_from_args(args, True) as sim:
        wrapper_path = sim.wrapper_path
        from ray import air, tune
        args.output = os.path.abspath(args.output)

        ppo_config, hyperopt_search = get_config(
            sim.wrapper_path,
            (
                1
                if args.true_self_play
                else sim.module.functions.get_num_players()
            ),
            league_play=args.league_play
        )
        tune.register_env(
            "rlc_env", lambda config: RLCEnvironment(wrapper_path=wrapper_path)
        )

        stop = {
            "timesteps_total": 1e15,
            # "episode_reward_mean": 2,  # divide by num_agents for actual reward per agent
        }

        # resumption_dir = os.path.abspath("./results")
        resources = PPO.default_resource_request(ppo_config)
        tuner = tune.Tuner(
            tune.with_resources(
                (
                    get_multi_train(sim.module.functions.get_num_players())
                    if args.league_play
                    else get_trainer(args.output, total_train_iterations=args.total_train_iterations)
                ),
                resources=resources,
            ),
            param_space=ppo_config.to_dict(),
            tune_config=ray.tune.TuneConfig(num_samples=args.sample_space, search_alg=hyperopt_search),
            run_config=air.RunConfig(
                stop=stop,
                verbose=2,
                # storage_path=resumption_dir
            ),
        )

        results = tuner.fit()


if __name__ == "__main__":
    main()
