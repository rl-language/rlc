import argparse
from loader.simulation import dump
from loader import Simulation, compile
from solvers import find_end
from shutil import which
from command_line import load_simulation_from_args, make_rlc_argparse


def main():
    parser = make_rlc_argparse("test", description="run all functions which their name starts with test")
    args = parser.parse_args()
    sim = load_simulation_from_args(args)

    failed = []
    for (name, overloads) in sim.module.wrappers.items():
        if not name.startswith("test_"):
            continue

        for overload in overloads:
            if len(sim.module.signatures[overload]) == 1 and sim.module.signatures[overload][0] == bool:
                print("")
                print(f"  RUN: {name}")
                result = overload()
                if result:
                    print(f"  OK:  {name}")
                else:
                    print(f"  KO:  {name}")
                    failed.append(name)

    if len(failed) != 0:
        print("\nFAILURES:")
    for failure in failed:
        print(f"  {failure}")
        exit(-1)

if __name__ == "__main__":
    main()
