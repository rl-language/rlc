import ray

import numpy as np
from ml.raylib.environment import RLCEnvironment
from ray.rllib.algorithms.algorithm import Algorithm
from ml.raylib.module_config import get_config

from command_line import load_simulation_from_args, make_rlc_argparse




def main():
    parser = make_rlc_argparse("probs", description="print the probability of executing each action")
    parser.add_argument("checkpoint", type=str)
    parser.add_argument("--no-one-agent-per-player", action="store_false", default=True)

    args = parser.parse_args()
    with load_simulation_from_args(args, optimize=True) as sim:
        wrapper_path = sim.wrapper_path

        from ray import air, tune
        tune.register_env(
            "rlc_env",
            lambda config: RLCEnvironment(wrapper_path=wrapper_path),
        )

        num_players = (
            1
            if not args.no_one_agent_per_player
            else sim.module.functions.get_num_players()
        )
        ppo_config, hyperopt_search = get_config(
            sim.wrapper_path, num_players, exploration=False
        )

        num_players = (
            1
            if not args.no_one_agent_per_player
            else sim.module.functions.get_num_players()
        )
        model = Algorithm.from_checkpoint(args.checkpoint)
        for i in range(num_players):
            model.workers.local_worker().module[f"p{i}"].load_state(
                f"{args.checkpoint}/learner/module_state/p{i}/module_state_dir/"
            )
        env = RLCEnvironment(wrapper_path=wrapper_path)
        obs, info = env.reset()
        i = 0
        while True:
            print(f"---------- {i} ----------------")
            env.wrapper.functions.pretty_print(env.state)
            print("--------- probs --------------")
            action = env.print_probs(model)
            print("------------------------------")
            observation, reward, done, truncated, info = env.step([action for i in range(env.num_agents)])
            i = i + 1
            if done["__all__"] or truncated["__all__"]:
                break

        env.wrapper.functions.pretty_print(env.state)



if __name__ == "__main__":
    main()
