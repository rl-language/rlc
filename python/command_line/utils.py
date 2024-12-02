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
from rlc import Program, State, compile
import pickle
import torch
import pathlib


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
        "--stdlib",
        type=str,
        nargs="?",
        help="path to stdlib",
        default=None,
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


def load_program_from_args(
    args, optimize=False, extra_source_files=[], gen_python_methods=True
):
    assert (
        which(args.rlc) is not None
    ), "could not find executable {}, use --rlc <path_to_rlc> to configure it".format(
        args.rlc
    )

    if args.source_file.endswith(".py"):
        return Program(args.source_file)
    else:
        includes = [include for include in args.include]
        return compile(
            [args.source_file] + extra_source_files,
            args.rlc,
            includes,
            args.runtime,
            optimized=optimize,
            stdlib=args.stdlib,
            gen_python_methods=gen_python_methods,
        )
