import ray
import os

import numpy as np
from ml.raylib.environment import RLCEnvironment, exit_on_invalid_env
from ray.rllib.algorithms.algorithm import Algorithm
from ml.raylib.module_config import get_config
from ml.raylib.environment import get_num_players
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from rlc import Program

from command_line import load_program_from_args, make_rlc_argparse


def main():
    parser = make_rlc_argparse(
        "probs", description="print the probability of executing each action"
    )
    parser.add_argument("checkpoint", type=str, nargs="?", default=None)
    parser.add_argument("--true-self-play", action="store_true", default=False)
    parser.add_argument("--pretty-print", "-pp", action="store_true", default=False)
    parser.add_argument("--state", "-s", default="")
    parser.add_argument("--iterations", default=1, type=int)

    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:
        exit_on_invalid_env(program)

        ray.init(
            num_cpus=12,
            num_gpus=1,
            include_dashboard=False,
            log_to_driver=False,
            logging_level="ERROR",
        )

        from ray import air, tune

        module_path = program.module_path

        tune.register_env(
            "rlc_env", lambda config: RLCEnvironment(program=Program(module_path))
        )

        num_players = get_num_players(program.module)
        num_agents = 1 if args.true_self_play else get_num_players(program.module)

        (config, _) = get_config(
            program, num_players, true_self_play=args.true_self_play
        )
        model = (
            Algorithm.from_checkpoint(args.checkpoint)
            if args.checkpoint is not None
            else config.build()
        )
        if args.checkpoint is not None:
            for i in range(num_agents):
                model.workers.local_worker().module[f"p{i}"].load_state(
                    f"{args.checkpoint}/learner/net_p{i}/"
                )
        os.system("cls||clear")
        for iteration in range(args.iterations):
            env = RLCEnvironment(program=program)
            state_to_load = args.state if args.state != "" else None
            obs, info = env.reset(path_to_binary_state=state_to_load)
            i = 0
            while True:
                if args.pretty_print:
                    os.system("cls||clear")
                print(f"---------- {i} : p{env.current_player} ------------")
                if args.pretty_print:
                    env.state.pretty_print()
                else:
                    env.state.print()
                print("--------- probs --------------")
                actions = env.print_probs(
                    model, policy_to_use=0 if args.true_self_play else None
                )
                action = actions[0]
                print("------------------------------")
                if args.pretty_print:
                    user_input = input()
                    action = (
                        actions[0] if user_input == "" else actions[int(user_input)]
                    )
                observation, reward, done, truncated, info = env.step(
                    [action for i in range(env.num_agents)]
                )
                i = i + 1
                if done["__all__"] or truncated["__all__"]:
                    break

            if args.pretty_print:
                env.state.pretty_print()
            else:
                env.state.print()


if __name__ == "__main__":
    main()
