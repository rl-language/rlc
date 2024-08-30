import ray

import os
import sys
import time
from statistics import mean
from tensorboard.backend.event_processing.event_accumulator import EventAccumulator
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

def extract_metric_from_logs(log_dir, metric_name):
    """Extracts time series data for a given metric from TensorBoard logs."""
    time_series_data = []
    steps = []

    # Load event data using EventAccumulator
    event_accumulator = EventAccumulator(log_dir)
    event_accumulator.Reload()

    # Check if the metric exists
    print(event_accumulator.Tags())
    fullname = f"ray/tune/env_runners/{metric_name}"
    if fullname in event_accumulator.Tags()['scalars']:
        # Retrieve all scalar events for the metric
        for event in event_accumulator.Scalars(fullname):
            print("called")
            steps.append(event.step)
            time_series_data.append(event.value)

    return steps, time_series_data

def plot_time_series(steps, values, title, filename):
    plt.figure(figsize=(10, 5))
    plt.plot(steps, values, label=title, color='blue')
    plt.title(title.replace("_", " "))
    plt.xlabel('Steps')
    plt.ylabel('Value')
    plt.grid(True)
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
    parser.add_argument("logdir", type=str)
    parser.add_argument("--true-self-play", action="store_true", default=False)

    args = parser.parse_args()
    with load_program_from_args(args, optimize=True) as program:
        metrics = []
        env = RLCEnvironment(program=program)
        for player in env.get_agent_ids():
            metrics.append(f"agent_episode_returns_mean/0")

        for name, metric in env.metrics_to_log.items():
            metrics.append(name)

        dir = TemporaryDirectory()
        images = []
        descriptions = []
        for name in metrics:
            steps, time_series_data = extract_metric_from_logs(args.logdir, name)
            escaped_path = name.replace('/', "_")
            file_name = f"{dir.name}/{escaped_path}.png"
            plot_time_series(steps, time_series_data, name, file_name)
            images.append(file_name)
            attr_name = f"description_{name}_training"
            if hasattr(program.functions, attr_name):
                descriptions.append(program.to_python_string(getattr(program.functions, attr_name)()))
            else:
                descriptions.append("")

        create_pdf_with_histograms(images, args.output, annotations=descriptions)



if __name__ == "__main__":
    main()
