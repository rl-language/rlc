#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#

import sys
import argparse
from loader import Simulation, compile, State
import os
from shutil import which
from command_line import load_simulation_from_args, make_rlc_argparse


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
        help="path to file containing a action for each line",
        default="-",
    )
    parser.add_argument("--ignore-invalid", "-ii", action="store_true", default=False)
    parser.add_argument("--print-all", "-all", action="store_true", default=False)
    parser.add_argument("--pretty-print", "-pp", action="store_true", default=False)
    parser.add_argument("--show-actions", "-a", action="store_true", default=False)

    args = parser.parse_args()
    with load_simulation_from_args(args) as sim:

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
        if args.pretty_print:
            state.simulation.module.functions.pretty_print(state.state)
            input()
        for i, line in enumerate(lines):
            if line.strip() == "" or line.strip().startswith("#"):
                continue

            if args.print_all:
                print(i, line)

            action = sim.parse_action(line)
            if action is None:
                if args.ignore_invalid:
                    continue
                else:
                    print("Cannot parse the following action:")
                    print(i, line)
                    break

            if not action.can_run(state):
                if args.ignore_invalid:
                    continue
                else:
                    print("Cannot apply the following action:")
                    failed = True
                    print(i, action, line)
                    break

            if args.pretty_print:
                input()
                os.system("cls||clear")
                state.simulation.module.functions.pretty_print(state.state)
            if not args.print_all:
                print(i, action)
            action.run(state)

        if args.pretty_print:
            input()
            os.system("cls||clear")
            state.simulation.module.functions.pretty_print(state.state)

        if args.output != "":
            state.write_binary(args.output)
        elif not args.pretty_print:
            print(state)

        if failed:
            exit(-1)


if __name__ == "__main__":
    main()
