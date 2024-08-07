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
from ml.raylib.module_config import get_config
from ml.raylib.environment import get_num_players

from rlc import Program
from command_line import load_program_from_args, make_rlc_argparse


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
    parser.add_argument("--true-self-play", action="store_true", default=False)
    parser.add_argument("--print-scores", action="store_true", default=False)
    parser.add_argument("--iterations", default=1, type=int)
    parser.add_argument("--progress", action="store_true", default=False)

    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:
        exit_on_invalid_env(program)

        ray.init(num_cpus=12, num_gpus=1, include_dashboard=False)

        from ray import air, tune

        module_path = os.path.abspath(program.module_path)
        tune.register_env(
            "rlc_env", lambda config: RLCEnvironment(program=Program(module_path))
        )

        num_players = (
            1 if args.true_self_play else get_num_players(program.module)
        )

        model = Algorithm.from_checkpoint(args.checkpoint)
        for i in range(num_players):
            model.workers.local_worker().module[f"p{i}"].load_state(
                f"{args.checkpoint}/learner/net_p{i}/"
            )
        out = open(args.output, "w+") if args.output != "" else sys.stdout
        for i in range(args.iterations):
            if args.progress:
                print(f"iteration {i}/{args.iterations}")
            out.write(f"# game: {i}\n")
            env = RLCEnvironment(program=program, solve_randomness=False)
            while env.current_player != -4:
                action = env.one_action_according_to_model(model, args.true_self_play)
                out.write(program.to_string(action) + "\n")
            if args.print_scores:
                out.write("# score: ")
                out.write(str(env.current_score))
                out.write("\n")


if __name__ == "__main__":
    main()
