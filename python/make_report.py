import ray

import os
import sys
import time
from statistics import mean
from tempfile import TemporaryDirectory
from collections import defaultdict
from ml.raylib.environment import RLCEnvironment, exit_on_invalid_env
from ray.rllib.env.multi_agent_env import make_multi_agent
from ray.rllib.algorithms import ppo
from ray.rllib.algorithms.algorithm import Algorithm
from ray.rllib.algorithms.ppo import PPOConfig, PPO
from ray.rllib.algorithms.ppo import PPOTorchPolicy
from ray import train
from ml.raylib.module_config import get_config
from ml.raylib.environment import get_num_players
import numpy as np
import matplotlib.pyplot as plt
from fpdf import FPDF

from rlc import Program
from command_line import load_program_from_args, make_rlc_argparse

def plot_histogram(observations, bin_width, title, filename):
    # Determine the bins based on the given bin width
    bins = np.arange(min(observations), max(observations) + bin_width, bin_width)

    # Plot the histogram
    plt.figure(figsize=(10, 5))
    plt.hist(observations, bins=bins, color='green', edgecolor='black')
    plt.title(title.replace("_", " "))
    plt.xlabel('End game value')
    plt.ylabel('Numer of playouts')
    plt.grid(True, axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename, format='png')
    plt.close()

def create_pdf_with_histograms(plot_files, output_pdf, annotations=None):
    pdf = FPDF()

    for i, plot_file in enumerate(plot_files):
        pdf.add_page()

        # Add the plot image
        pdf.image(plot_file, x=10, y=10, w=190)

        # Add annotations
        if annotations is not None:
            pdf.set_xy(10, 100)
            pdf.set_font('Arial', 'b', 12)
            pdf.multi_cell(0, 10, annotations[i])

    pdf.output(output_pdf)


def main():
    parser = make_rlc_argparse("train", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="out.pdf",
    )
    parser.add_argument("checkpoint", type=str)
    parser.add_argument("--true-self-play", action="store_true", default=False)
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

        num_agents = 1 if args.true_self_play else get_num_players(program.module)

        model = Algorithm.from_checkpoint(args.checkpoint)
        for i in range(num_agents):
            model.workers.local_worker().module[f"p{i}"].load_state(
                f"{args.checkpoint}/learner/net_p{i}/"
            )
        metrics = defaultdict(list)
        means = []
        for i in range(args.iterations):
            if args.progress:
                print(f"iteration {i}/{args.iterations}")

            env = RLCEnvironment(program=program)
            while env.current_player != -4:
                action = env.one_action_according_to_model(model, args.true_self_play)
            for player in range(env.num_players):
                metrics[f"score_p{player}"].append(env.score(player))

            for name, metric in env.metrics_to_log.items():
                metrics[name].append(metric(env.state.state))

        dir = TemporaryDirectory()
        images = []
        descriptions = []
        for name, metric in metrics.items():
            file_name = f"{dir.name}/{name}.png"
            plot_histogram(metric, 1, name, file_name)
            images.append(file_name)
            descriptions.append(f"average: {mean(metric)}\n")
            attr_name = f"description_{name}"
            if hasattr(program.functions, attr_name):
                descriptions[-1] = descriptions[-1] + program.to_python_string(getattr(program.functions, attr_name)())

        create_pdf_with_histograms(images, args.output, annotations=descriptions)



if __name__ == "__main__":
    main()
