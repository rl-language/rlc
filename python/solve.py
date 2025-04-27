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
import random
import sys
from rlc import Program, compile, State
from shutil import which
from ml.ppg.envs import exit_on_invalid_env
from command_line import load_program_from_args, make_rlc_argparse


def main():
    parser = make_rlc_argparse("solve", description="runs a action of the simulation")
    parser.add_argument(
        "--output",
        "-o",
        type=str,
        nargs="?",
        help="path where to write the output",
        default="",
    )
    parser.add_argument(
        "--max-actions",
        "-m",
        type=int,
        nargs="?",
        help="max number of actions to execute before exiting",
        default=-1,
    )

    parser.add_argument(
        "--iterations",
        type=int,
        nargs="?",
        help="how many games must be played out",
        default=1,
    )

    parser.add_argument(
        "--initial-state",
        type=str,
        help="Initial state",
        default="",
    )

    args = parser.parse_args()
    with load_program_from_args(args) as program:
        if not program.module.print_enumeration_errors(
            program.module.AnyGameAction()
        ):
            exit(-1)
        exit_on_invalid_env(program, forced_one_player=True, needs_score=False)
        state = program.start()

        out = open(args.output, "w+") if args.output != "" else sys.stdout
        current = 0
        while args.iterations != current:
            out.write(f"# game {current}\n")
            current = current + 1
            state.reset()

            if args.initial_state != "":
                if not state.load_string_from_file(args.initial_state):
                    print("failed to load initial state")
                    exit(-1)
            while not state.is_done():
                if args.max_actions != -1:
                    if args.max_actions == 0:
                        exit()
                    args.max_actions = args.max_actions - 1
                actions = state.legal_actions
                if len(actions) == 0:
                    print("found state with no valid action")
                    state.print()
                    exit(-1)
                action = random.choice(actions)
                state.step(action)
                out.write(str(action))
                out.write("\n")
                out.flush()


if __name__ == "__main__":
    main()
