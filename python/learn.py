import ray
import pickle
import copy
import math
import time
import sys
import os
import tempfile
import random

from tensorboard.program import TensorBoard
from ml.raylib.environment import RLCEnvironment, exit_on_invalid_env
from ray.rllib.env.multi_agent_env import make_multi_agent
from importlib import import_module, machinery, util
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray import train
from ml.raylib.module_config import get_config

from command_line import load_program_from_args, make_rlc_argparse
from rlc import Program
from ml.raylib.environment import get_num_players

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


def trial_str_creator(trial):
    return str(trial.trial_id)


def check_ray_bug():
    import ray.rllib.algorithms.ppo.ppo as ppo

    file_path = os.path.join(os.path.dirname(ppo.__file__), "ppo.py")
    found = False
    with open(file_path, "r") as file:
        contents = file.readlines()
        for line in contents:
            if "default=0," in line:
                found = True
    if not found:
        print(
            "The installed version of Ray, a library RLC depends upon, is known to be bugged.\nRun rlc-fix-ray to patch it. This will modify the installed file."
        )
        exit(-1)


def save(model, output_path, num_players):
    if output_path != "":
        model.save(output_path)
        for player_id in range(num_players):
            if not os.path.exists(
                os.path.join(output_path, "learner", f"net_p{player_id}")
            ):
                os.makedirs(os.path.join(output_path, "learner", f"net_p{player_id}"))
            model.workers.local_worker().module[f"p{player_id}"].save_state(
                os.path.join(output_path, "learner", f"net_p{player_id}")
            )
            model.workers.local_worker().module[f"p{player_id}"].load_state(
                os.path.join(output_path, "learner", f"net_p{player_id}")
            )


def get_multi_train(num_players, output):
    def train_impl(config, fixed_nets=0):
        config = PPOConfig().update_from_dict(config)
        model = config.build()

        for _ in range(1000000000):
            with open(f"list_of_fixed.txt", "wb+") as f:
                for i in range(num_players):
                    if not os.path.exists(f"net_p{num_players + i}_{fixed_nets}"):
                        os.makedirs(f"net_p{num_players + i}_{fixed_nets}")
                    model.workers.local_worker().module[f"p{i}"].save_state(
                        f"net_p{num_players + i}_{fixed_nets}"
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
                                f"net_p{i + num_players}_{random.choice(range(fixed_nets))}"
                            )
                        model.workers.sync_weights(
                            policies=[f"p{i + num_players}" for i in range(num_players)]
                        )
            model.save(f"checkpoint")
            save(model, output, num_players)
        model.save()
        model.stop()

    return train_impl


def get_trainer(output_path, total_train_iterations, num_players):
    def single_agent_train(config):
        config = PPOConfig().update_from_dict(config)
        model = config.build()
        for _ in range(math.ceil(total_train_iterations / 10)):
            for i in range(10):
                train.report(model.train())
            save(model, output_path, num_players)

        model.save()
        model.stop()

    return single_agent_train


def ray_count_alive_nodes():
    return len(
        [
            node
            for node in ray.nodes()
            if node["Alive"]
            and node["NodeManagerAddress"]
            != ray._private.services.get_node_ip_address()
        ]
    )

def load_initial_states(program, init_state_dir):
    initial_states = []
    if init_state_dir == "":
        return initial_states

    if not os.path.isdir(init_state_dir):
        print(f"initial states directory {init_state_dir} is not a directory")
        exit(-1)

    for filename in os.listdir(init_state_dir):
        with open(f"{init_state_dir}/{filename}", "r") as file:
            content = "".join(file.readlines())
            state = program.module.Game()
            if not program.functions.from_string(state, program.to_rl_string(content)).value:
                print(f"failed to parse {file}")
                exit(-1)
            initial_states.append(state)

    return initial_states

def make_env(program_path, initial_states_path):
    program = Program(program_path)
    return RLCEnvironment(program=program, initial_states=load_initial_states(program, initial_states_path))

def main():
    check_ray_bug()
    parser = make_rlc_argparse("train", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="network",
    )
    parser.add_argument("--true-self-play", action="store_true", default=False)
    parser.add_argument("--league-play", action="store_true", default=False)
    parser.add_argument("--initial-states", type=str, default="")
    parser.add_argument("--no-tensorboard", action="store_true", default=False)
    parser.add_argument("--total-train-iterations", default=100000000, type=int)
    parser.add_argument("--num-sample", default=1, type=int)
    parser.add_argument("--num-rollout-cpus", default=8, type=int, help="num of cpus taskes with playing the game while training, reduce this number to reduce ram usage, but increase trying time.")

    args = parser.parse_args()
    program = load_program_from_args(args, True)
    if not program.functions.print_enumeration_errors(program.module.AnyGameAction()):
        exit(-1)
    program.functions.emit_observation_tensor_warnings(program.functions.play())
    exit_on_invalid_env(program)
    load_initial_states(program, args.initial_states)
    module_path = os.path.abspath(program.module_path)

    ray.init(num_cpus=12, num_gpus=1, include_dashboard=False, log_to_driver=False)
    session_dir = ray.worker._global_node.get_session_dir_path()
    print(f"SESSION_DIR: {session_dir}")
    from ray import air, tune

    args.output = os.path.abspath(args.output)

    num_players = get_num_players(program.module)
    num_agents = 1 if args.true_self_play else get_num_players(program.module)

    ppo_config, hyperopt_search = get_config(
        program,
        num_players,
        league_play=args.league_play,
        true_self_play=args.true_self_play,
        num_rollout_workers=args.num_rollout_cpus
    )

    initial_args_dir = args.initial_states if args.initial_states == "" else os.path.abspath(args.initial_states)
    tune.register_env(
        "rlc_env", lambda config: make_env(module_path, initial_args_dir)
    )

    stop = {
        "timesteps_total": 1e15,
        # "episode_reward_mean": 2,  # divide by num_agents for actual reward per agent
    }

    # resumption_dir = os.path.abspath("./results")
    resources = PPO.default_resource_request(ppo_config)

    air_config = air.RunConfig(
        stop=stop,
        verbose=2,
        # storage_path=resumption_dir
    )

    tuner = tune.Tuner(
        tune.with_resources(
            (
                get_multi_train(num_agents, args.output)
                if args.league_play
                else get_trainer(
                    args.output,
                    total_train_iterations=args.total_train_iterations,
                    num_players=num_agents,
                )
            ),
            resources=resources,
        ),
        param_space=ppo_config.to_dict(),
        tune_config=ray.tune.TuneConfig(
            num_samples=args.num_sample,
            search_alg=hyperopt_search,
            trial_name_creator=trial_str_creator,
            trial_dirname_creator=trial_str_creator,
        ),
        run_config=air.RunConfig(
            stop=stop,
            verbose=2,
            # storage_path=resumption_dir
        ),
    )

    if not args.no_tensorboard:
        tb = TensorBoard()
        tb.configure(argv=[None, "--logdir", session_dir])
        url = tb.launch()

    results = tuner.fit()
    while ray_count_alive_nodes() != 0:
        print(ray_count_alive_nodes(), "alive nodes")
        time.sleep(1)
    ray.shutdown()
    time.sleep(3)
    program.cleanup()


if __name__ == "__main__":
    main()
