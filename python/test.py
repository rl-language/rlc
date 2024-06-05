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
import time
from loader import Simulation, compile
from shutil import which
from command_line import load_simulation_from_args, make_rlc_argparse


def main():
    parser = make_rlc_argparse(
        "test", description="run all functions which their name starts with test"
    )
    args = parser.parse_args()
    with load_simulation_from_args(args) as sim:

        failed = []
        for name, overloads in sim.module.wrappers.items():
            if not name.startswith("test_"):
                continue

            for overload in overloads:
                if (
                    len(sim.module.signatures[overload]) == 1
                    and sim.module.signatures[overload][0] == bool
                ):
                    print(f"  RUN: {name}")
                    start_time = time.perf_counter()
                    result = overload()
                    end_time = time.perf_counter()
                    if result:
                        print(f"  OK:  {name} {end_time-start_time:.3g}s")
                    else:
                        print(f"  KO:  {name}")
                        failed.append(name)
                    print("")

        if len(failed) != 0:
            print("\nFAILURES:")
        for failure in failed:
            print(f"  {failure}")
            exit(-1)


if __name__ == "__main__":
    main()
