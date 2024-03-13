#
# Copyright 2024 Massimo Fioravanti
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import argparse
import os
from loader import Simulation, compile
from solvers import find_end, explore, ml_mcts, play_game, all_actions
from ml import ActionOracle, RLCTransformer
from command_line import load_simulation_from_args, make_rlc_argparse, load_network
import pickle
import torch
from tempfile import TemporaryDirectory


def organize_simulations_result(action_table, entry):
    list_to_save = []
    for (state, moves_and_scores) in entry:
        list_moves = []
        print(str(state))
        for move_score in moves_and_scores:
            action = move_score.action
            print(action, str(action_table[action]), move_score.score)
            list_moves.append((action, move_score.score))

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

    state = sim.start("play")
    # find_end(Simulation, state)

    transformer = load_network(args.network, 256, "cpu")
    action_table = [action for action in all_actions(sim)]
    oracle = ActionOracle(transformer, action_table)

    transformer.model.eval()  # turn on evaluation mode
    with torch.no_grad():
        with torch.inference_mode():
            with open(args.output, "wb+" if not args.append else "ab") as f:
                for entry in ml_mcts(
                    lambda: sim.start("play"), oracle, args.turn_limit, 200, args.runs
                ):
                    pickle.dump(organize_simulations_result(action_table, entry), f)


if __name__ == "__main__":
    main()
