import ray

import os
import sys
import time
from ml.raylib.environment import RLCEnvironment, exit_on_invalid_env
from ray.rllib.env.multi_agent_env import make_multi_agent
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray import train
from ml.raylib.module_config import  get_config

from loader.simulation import import_file
from command_line import load_simulation_from_args, make_rlc_argparse


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
    parser.add_argument("--no-one-agent-per-player", action="store_false", default=True)
    parser.add_argument("--print-scores", action="store_true", default=False)
    parser.add_argument("--iterations", default=1, type=int)

    args = parser.parse_args()
    with load_simulation_from_args(args, optimize=True) as sim:
        exit_on_invalid_env(sim)

        ray.init(num_cpus=12, num_gpus=1, include_dashboard=False)

        from ray import air, tune
        wrapper_path = os.path.abspath(sim.wrapper_path)
        tune.register_env(
            "rlc_env", lambda config: RLCEnvironment(wrapper=import_file("dc", wrapper_path))
        )

        num_players = (
            1
            if not args.no_one_agent_per_player
            else sim.module.functions.get_num_players()
        )

        model = Algorithm.from_checkpoint(args.checkpoint)
        for i in range(num_players):
            model.workers.local_worker().module[f"p{i}"].load_state(
                f"{args.checkpoint}/learner/net_p{i}/"
            )
        out = open(args.output, "w+") if args.output != "" else sys.stdout
        for i in range(args.iterations):
          env = RLCEnvironment(wrapper=sim.module, solve_randomness=False)
          while env.current_player() != -4:
            action = env.one_action_according_to_model(model)
            out.write(env.action_to_string(action) + "\n")
          if args.print_scores:
            out.write(str(env.current_score))
            out.write('\n')



if __name__ == "__main__":
    main()
