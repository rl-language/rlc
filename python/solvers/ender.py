from loader import Simulation, State, Action
from itertools import product
from typing import Tuple, Iterator


def find_viable_actions(state: State) -> Iterator[Tuple[Action, list[int]]]:
    for action in state.actions:
        min_max = [argument.get_min_max() for argument in action.args[1:]]

        for args in product(*[range(begin, end + 1) for begin, end in min_max]):
            if not action.can_run(state, *args):
                continue

            yield (action, args)


def execute_action(simulation: Simulation, state: State) -> bool:
    viable_actions = [(x, y) for (x, y) in find_viable_actions(state)]
    if len(viable_actions) == 0:
        return False

    (action, args) = viable_actions[0]
    print(action.name, args)
    action.run(state, *args)
    return True


def find_end(simulation: Simulation, state: State):
    while not state.is_done():
        assert execute_action(simulation, state)
        state.dump()


def explore(simulation: Simulation, state: State, output: [Tuple[State, int]]) -> int:
    print(len(output))
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
