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
from typing import List, Tuple, Dict
from loader import State, Action
from dataclasses import dataclass
from solvers import find_viable_actions, all_actions


@dataclass
class MoveAndScore:
    action: int
    score: float


# takes a list of states, a move oracle and the cut off turn limit and returns the expected value of each state obtained by playing the game once
def single_playout(states: [State], moves_oracle, turn_limit=100) -> [float]:
    turn_counter = 0
    while (
        not all(state is None or state.is_done() for state in states)
        and turn_counter < turn_limit
    ):
        turn_counter = turn_counter + 1
        actions = moves_oracle(states)
        states = [(None if action is None else action.run(state)) for (action, state) in zip(actions, states)]

    to_return = []
    for state in states:
        if state is None:
            to_return.append(None)
        else:
            to_return.append(float(state.score()))
    return to_return


# takes a state, a move oracle, the cut off turn limit and returns the expected value of each state obtained by averaging out the result of playing each game samples number of times. does not modify states
def playout(state: State, moves_oracle, turn_limit=100, samples=100) -> float:
    copied_states = [state.copy() for x in range(samples)]
    values = single_playout(copied_states, moves_oracle, turn_limit)
    values = [x for x in values if x is not None]
    if len(values) == 0:
        return None
    return sum(values) / len(values)


# takes a state, a move oracle, a turn limit, the number of roll outs and the number of samples and returns a list that maps each move choses by monte carlo to the expected result, obtained by playing the name the given number of rollouts.
def sample_expected_value(
    state, moves_oracle, turn_limit=100, rollouts=100
) -> [MoveAndScore]:
    to_return = []

    for (index, action) in enumerate(all_actions(state.simulation)):
        if not action.can_run(state):
            to_return.append(MoveAndScore(index, -1.0))
            continue

        copied_state = state.copy()
        action.run(copied_state)
        score = playout(copied_state, moves_oracle, turn_limit, rollouts)
        if score is not None:
            to_return.append(MoveAndScore(index, score))

    return to_return


def play_game(
    state, moves_oracle, turn_limit=100, samples_size=100
) -> [(State, [MoveAndScore])]:
    turn_counter = 0
    to_return = []
    while state is not None and not state.is_done() and turn_counter < turn_limit:
        samples = sample_expected_value(state, moves_oracle, turn_limit, samples_size)
        to_return.append((state, samples))

        move = moves_oracle([state])[0]
        state = None if move is None else move.run(state.copy())
        turn_counter = turn_counter + 1

    return to_return


def ml_mcts(
    initial_state_generator,
    moves_oracle,
    turn_limit=100,
    samples_size=100,
    total_games=2000,
) -> [(State, [MoveAndScore])]:
    for i in range(total_games):
        print(i)
        yield play_game(initial_state_generator(), moves_oracle)
