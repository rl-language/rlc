#
# This file is part of the RLC project.
#
# RLC is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.
#
# RLC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with RLC. If not, see <https://www.gnu.org/licenses/>.
#
from loader import Simulation, State, Action
from itertools import product
from typing import Tuple, Iterator

def all_actions(simulation: Simulation) -> Iterator[Action]:
    for action in simulation.actions:
        min_max = action.get_arg_min_maxs()

        for args in product(*[range(begin, end) for begin, end in min_max]):
            yield action.materialize(*args)


def find_viable_actions(state: State) -> Iterator[Action]:
    for action in all_actions(state.simulation):
        if not action.can_run(state):
            continue

        yield action


def execute_action(simulation: Simulation, state: State) -> bool:
    viable_actions = [x for x in find_viable_actions(state)]
    if len(viable_actions) == 0:
        return False

    action = viable_actions[0]
    action.run(state)
    return True


def find_end(simulation: Simulation, state: State):
    while not state.is_done():
        assert execute_action(simulation, state)
        print(state)


def explore(simulation: Simulation, state: State, output: [Tuple[State, int]]) -> int:
    if state.is_done():
        output.append((state, state.state.size))
        return state.state.size

    min_value = 999
    for (action, args) in find_viable_actions(state):
        new_one = state.copy()
        action.run(new_one, *args)
        child_score = explore(simulation, new_one, output)
        min_value = min_value if min_value < child_score else child_score

    output.append((state, min_value))
    return min_value
