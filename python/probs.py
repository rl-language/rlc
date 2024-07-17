import ray
import os

import numpy as np
from ml.raylib.environment import RLCEnvironment, exit_on_invalid_env
from ray.rllib.algorithms.algorithm import Algorithm
from ml.raylib.module_config import get_config
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from loader.simulation import import_file

from command_line import load_simulation_from_args, make_rlc_argparse




def main():
    parser = make_rlc_argparse("probs", description="print the probability of executing each action")
    parser.add_argument("checkpoint", type=str)
    parser.add_argument("--true-self-play", action="store_true", default=False)
    parser.add_argument("--pretty-print", "-pp", action="store_true", default=False)
    parser.add_argument("--state", "-s", default="")
    parser.add_argument("--iterations",  default=1, type=int)

    args = parser.parse_args()
    with load_simulation_from_args(args, optimize=True) as sim:
        exit_on_invalid_env(sim)

        ray.init(num_cpus=12, num_gpus=1, include_dashboard=False, log_to_driver=False, logging_level="ERROR")

        from ray import air, tune
        wrapper_path = sim.wrapper_path

        tune.register_env(
            "rlc_env", lambda config: RLCEnvironment(wrapper=import_file("dc", wrapper_path))
        )

        num_players = (
            1
            if args.true_self_play
            else sim.module.functions.get_num_players()
        )

        model = Algorithm.from_checkpoint(args.checkpoint)
        for i in range(num_players):
            model.workers.local_worker().module[f"p{i}"].load_state(
                f"{args.checkpoint}/learner/net_p{i}/"
            )
        for iteration in range(args.iterations):
          env = RLCEnvironment(wrapper=sim.module)
          state_to_load = args.state if args.state != "" else None
          obs, info = env.reset(path_to_binary_state=state_to_load)
          i = 0
          while True:
            if args.pretty_print:
                os.system("cls||clear")
            print(f"---------- {i} : p{env.current_player()} ------------")
            if args.pretty_print:
                env.wrapper.functions.pretty_print(env.state)
            else:
                env.wrapper.functions.print(env.state)
            print("--------- probs --------------")
            actions = env.print_probs(model, policy_to_use=0 if args.true_self_play else None)
            action = actions[0]
            print("------------------------------")
            if args.pretty_print:
                user_input = input()
                action = actions[0] if user_input == "" else actions[int(user_input)]
            observation, reward, done, truncated, info = env.step([action for i in range(env.num_agents)])
            i = i + 1
            if done["__all__"] or truncated["__all__"]:
                break

          env.wrapper.functions.pretty_print(env.state)



if __name__ == "__main__":
    main()
