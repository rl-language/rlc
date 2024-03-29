#
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
import os
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
    parser.add_argument("--ignore-invalid", "-ii", action="store_true", default=False)
    parser.add_argument("--print-all", "-all", action="store_true", default=False)
    parser.add_argument("--show-actions", "-a", action="store_true", default=False)

    args = parser.parse_args()
    sim = load_simulation_from_args(args)

    if args.show_actions:
        sim.dump()
        return

    state = sim.start("play")
    if args.load != "":
        state.load(args.load)

    failed = False

    lines = (
        sys.stdin.readlines()
        if args.action_file == "-"
        else open(args.action_file, "r").readlines()
    )
    for line in lines:
        if line.strip() == "" or line.strip().startswith("#"):
            continue

        if args.print_all:
            print(line)

        action = sim.parse_action(line)
        if action is None:
            if args.ignore_invalid:
                continue
            else:
                print("Cannot parse the following action:")
                print(line)
                break

        if not action.can_run(state):
            if args.ignore_invalid:
                continue
            else:
                print("Cannot apply the following action:")
                failed = True
                print(action)
                break

        if not args.print_all:
            print(action)
        action.run(state)

    if args.output != "":
        state.write_binary(args.output)
    else:
        print(state)

    if failed:
        exit(-1)


if __name__ == "__main__":
    main()
