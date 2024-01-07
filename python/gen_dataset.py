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
from loader.simulation import dump
import os
from loader import Simulation, compile
from solvers import find_end, explore, ml_mcts, play_game
from ml import ActionOracle, RLCTransformer
from command_line import load_simulation_from_args, make_rlc_argparse, load_network
import pickle
import torch
from tempfile import TemporaryDirectory


def organize_simulations_result(entry):
    list_to_save = []
    for (state, moves_and_scores) in entry:
        list_moves = []
        state.dump()
        for move_score in moves_and_scores:
            action = move_score.action
            args = move_score.args
            byte_array = state.simulation.args_as_canonical_raw_bytes(action, args)
            print(action.name, args, move_score.score)
            list_moves.append((bytes(byte_array), move_score.score))

        list_to_save.append((state.as_byte_vector(), list_moves))
    return list_to_save


def main():
    parser = make_rlc_argparse(
        "gen_dataset", description="runs a action of the simulation"
    )
    parser.add_argument("--network", "-n", type=str, default="")
    parser.add_argument("output", type=str)
    parser.add_argument("--append", "-a", action="store_true")
    parser.add_argument("--runs", "-r", type=int, default=200)
    parser.add_argument("--turn-limit", "-tl", type=int, default=10)
    args = parser.parse_args()
    sim = load_simulation_from_args(args)

    state = sim.start(["play"])
    # find_end(Simulation, state)

    transformer = load_network(args.network, 256, "cpu")
    oracle = ActionOracle(transformer)

    transformer.model.eval()  # turn on evaluation mode
    with torch.no_grad():
        with torch.inference_mode():
            with open(args.output, "wb+" if not args.append else "ab") as f:
                for entry in ml_mcts(
                    lambda: sim.start(["play"]), oracle, args.turn_limit, 200, args.runs
                ):
                    pickle.dump(organize_simulations_result(entry), f)


if __name__ == "__main__":
    main()
