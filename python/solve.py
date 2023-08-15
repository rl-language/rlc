import argparse
from loader.simulation import dump
from loader import Simulation, compile
from solvers import find_end
from shutil import which
from command_line import load_simulation_from_args, make_rlc_argparse


def main():
    parser = make_rlc_argparse("solve", description="runs a action of the simulation")
    args = parser.parse_args()
    sim = load_simulation_from_args(args)

    state = sim.start(["play"])

    state.dump()
    find_end(sim, state)


if __name__ == "__main__":
    main()
