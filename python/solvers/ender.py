from loader import Simulation, State, Action
from itertools import product


def execute_action(simulation: Simulation, state: State) -> bool:
    for action in state.actions:
        min_max = [argument.get_min_max() for argument in action.args[1:]]

        for args in product(*[range(begin, end + 1) for begin, end in min_max]):
            if not action.can_run(state, *args):
                continue

            print(action.name, args)
            action.run(state, *args)
            return True

        return False


def find_end(simulation: Simulation, state: State):
    while not state.is_done():
        assert execute_action(simulation, state)
        state.dump()
