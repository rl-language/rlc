#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from typing import List, Tuple, Dict
from loader import State, Action
from dataclasses import dataclass
from solvers import find_viable_actions


@dataclass
class MoveAndScore:
    action: Action
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
        states = [action(state) for (action, state) in zip(actions, states)]

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

    for action in find_viable_actions(state):
        copied_state = state.copy()
        action.run(copied_state)
        score = playout(copied_state, moves_oracle, turn_limit, rollouts)
        if score is not None:
            to_return.append(MoveAndScore(action, score))

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
        state = move(state.copy())
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
