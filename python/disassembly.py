# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
import argparse
from loader import Simulation, compile, State
from solvers import find_end
import sys
from shutil import which
from command_line import load_simulation_from_args, make_rlc_argparse, load_network


def main():
    parser = make_rlc_argparse("solve", description="runs a action of the simulation")
    parser.add_argument(
        "--load",
        "-l",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="",
    )
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="",
    )
    parser.add_argument(
        "action_file",
        type=str,
        nargs="?",
        help="path to file containing a action for each line",
        default="-",
    )
    args = parser.parse_args()
    sim = load_simulation_from_args(args)

    lines = (
        sys.stdin.read()
        if args.action_file == "-"
        else open(args.action_file, "rb").read()
    )
    output = sys.stdout if args.output == "" else open(args.output, "w")
    actions = sim.parse_actions_from_binary_buffer(lines)
    for action in actions:
        string = sim.to_python_string(sim.module.functions.to_string(action))
        output.write(string)
        output.write("\n")


if __name__ == "__main__":
    main()
