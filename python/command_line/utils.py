#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from shutil import which
import argparse
from loader import Simulation, compile
import pickle
from ml import RLCTransformer
import torch


def load_network(path, ntokens, device):
    if path == "":
        return RLCTransformer(ntokens=ntokens, device=device)
    return torch.load(path)


def load_dataset(path):
    to_return = []
    with open(path, "rb") as f:
        while True:
            try:
                loaded = pickle.load(f)
                for state, actions_and_scores in loaded:
                    to_return.append((state, actions_and_scores))
            except Exception as e:
                break
    return to_return


def make_rlc_argparse(name, description):
    parser = argparse.ArgumentParser(name, description=description)
    parser.add_argument(
        "source_file",
        type=str,
        help="path to .rl source file, or to a python wrapper obtained with rlc --python",
    )
    parser.add_argument(
        "--include",
        "-i",
        type=str,
        nargs="*",
        help="path to folder where rl files can be found",
        default=[],
    )
    parser.add_argument(
        "--rlc",
        "-c",
        type=str,
        nargs="?",
        help="path to rlc compiler",
        default="rlc",
    )
    parser.add_argument(
        "--runtime",
        "-rt",
        type=str,
        default="",
        nargs="?",
        help="path to runtime library",
    )
    return parser


def load_simulation_from_args(args, optimize=False):
    assert (
        which(args.rlc) is not None
    ), "could not find executable {}, use --rlc <path_to_rlc> to configure it".format(
        args.rlc
    )

    if args.source_file.endswith(".py"):
      return Simulation(args.source_file)
    else:
      return compile(args.source_file, args.rlc, args.include, args.runtime, optimized=optimize)
