from importlib import import_module, machinery, util
import inspect
from collections import defaultdict
from tempfile import TemporaryDirectory
from subprocess import run
from ctypes import Structure, Array


def dump(generated_struct, indendation=0):
    if isinstance(generated_struct, Structure):
        print()
        for field_name, field_type in generated_struct._fields_:
            print((" " * indendation) + field_name[1:] + ": ", end="")
            dump(getattr(generated_struct, field_name), indendation + 1)
            print()
        return

    if (
        isinstance(generated_struct, bool)
        or isinstance(generated_struct, int)
        or isinstance(generated_struct, float)
    ):
        print(generated_struct, end="")
        return

    if isinstance(generated_struct, Array):
        for i in range(len(generated_struct)):
            dump(generated_struct[i], indendation + 1)
            print(", ", end="")
        return

    print(generated_struct, end="")


def import_file(name, file_path):
    loader = machinery.SourceFileLoader(name, file_path)
    spec = util.spec_from_loader(name, loader)
    mod = util.module_from_spec(spec)
    loader.exec_module(mod)
    return mod


class Argument:
    def __init__(self, action, arg_index):
        self.action = action
        self.index = arg_index

    @property
    def name(self):
        return inspect.getfullargspec(self.action.action)[0][self.index]

    def get_min_max(self):
        arg_info = self.action.module.module.args_info
        if self.action.action not in arg_info:
            return None

        return arg_info[self.action.action][self.index]

    @property
    def max(self):
        maybe_min_max = self.get_min_max()
        if maybe_min_max is None:
            return None
        (_, max) = maybe_min_max
        return max

    @property
    def min(self):
        maybe_min_max = self.get_min_max()
        if maybe_min_max is None:
            return None
        (min, _) = maybe_min_max
        return min

    def dump(self):
        print("\targ: {}".format(self.name))
        if self.min is not None:
            print("\t\tmin: {}".format(self.min))
        if self.min is not None:
            print("\t\tmax: {}".format(self.max))

    @property
    def type(self):
        return self.action.arg_types[self.index]

    def parse(self, to_convert):
        if isinstance(to_convert, self.type):
            return to_convert

        if not isinstance(to_convert, str):
            print(
                "Unable to convert argument {} to type {}",
                to_conver,
                type(to_convert).__name__(),
            )
            return None

        if self.type == int:
            return int(to_convert)

        if self.type == bool:
            return bool(to_convert)

        if self.type == float:
            return float(to_convert)

        print("Conversion to non primitive types is not implemented yet")
        assert false


class Action:
    def __init__(self, action, precondition, name: str, module):
        self.action = action
        self.name = name
        self.module = module
        self.precondition = precondition

        arg_info = self.module.module.args_info[self.action]
        self.args = [Argument(self, i) for i in range(len(arg_info))]

    @property
    def return_type(self):
        return self.module.module.signatures[self.action][0]

    @property
    def arg_types(self):
        return self.module.module.signatures[self.action][1:]

    @property
    def signature(self):
        return self.module.module.signatures[self.action]

    def get_simulation_init(self):
        return self.module.action_to_simulation_init[self]

    def is_simulation_init(self) -> bool:
        return self.get_simulation_init() == self

    def dump(self):
        if self.return_type is not None:
            print(
                "{}({}) -> {}".format(
                    self.name,
                    ", ".join(type.__name__ for type in self.arg_types),
                    self.return_type.__name__,
                )
            )
        else:
            print(
                "{}({})".format(
                    self.name, ", ".join(type.__name__ for type in self.arg_types)
                )
            )
        for arg in self.args:
            arg.dump()

    def can_run(self, state, *args) -> bool:
        return self.precondition(state.state, *args)

    def run(self, state, *args):
        return self.action(state.state, *args)

    def invoke(self, *args):
        casted_args = [
            formal_arg.parse(string_arg)
            for (formal_arg, string_arg) in zip(self.args, args)
        ]
        if None in casted_args:
            return

        return self.action(*casted_args)


class State:
    def __init__(self, simulation, state):
        self.simulation = simulation
        self.state = state

    def execute(self, *arguments):
        return self.simulation.execute([arguments[0], self.state, *arguments[1:]])

    def copy(self):
        return State(self.simulation, self.state.copy())

    def is_done(self) -> bool:
        return self.state.resume_index == -1

    def dump(self):
        dump(self.state)

    @property
    def actions(self) -> [Action]:
        return [
            action
            for action in self.simulation.actions
            if len(action.arg_types) != 0 and action.arg_types[0] == type(self.state)
        ]


class Simulation:
    def __init__(self, wrapper: str):
        self.wrapper_path = wrapper
        self.module = import_file("sim", wrapper)

        self.actions = []
        for action_name in self.action_names:
            for overload in self.module.actions[action_name]:
                precodition = [
                    action
                    for action in self.module.wrappers["can_" + action_name]
                    if self.module.signatures[action][1:]
                    == self.module.signatures[overload][1:]
                ]
                assert len(precodition) <= 1
                print(action_name, len(precodition))
                self.actions.append(
                    Action(
                        overload,
                        precodition[0] if len(precodition) == 1 else lambda *x: True,
                        action_name,
                        self,
                    )
                )

        self.action_to_simulation_init = {}
        self.entity_type_to_simulation_init = {}
        self.simulation_inits = []
        self.names_to_overloads = defaultdict(list)

        for action in self.actions:
            if action.return_type is None:
                continue

            self.simulation_inits.append(action)
            self.entity_type_to_simulation_init[action.return_type] = action
            self.action_to_simulation_init[action] = action
            self.names_to_overloads[action.name].append(action)

        for action in self.actions:
            if not action.return_type is None:
                continue

            entity_type = action.arg_types[0]
            self.action_to_simulation_init[
                action
            ] = self.entity_type_to_simulation_init[entity_type]
            self.names_to_overloads[action.name].append(action)

    def get_actions(self) -> [Action]:
        return self.actions

    @property
    def action_names(self) -> [str]:
        return [name for name in self.module.actions.keys()]

    def get_overloads_of_action(self, action_name: str) -> [Action]:
        return self.names_to_overloads[action_name]

    def dump(self):
        for name in self.action_names:
            for overload in self.get_overloads_of_action(name):
                overload.dump()

    def execute(self, arguments, include_simulations_init=False):
        assert len(arguments) != 0
        action_name = arguments[0]
        args = arguments[1:]

        overloads = self.get_overloads_of_action(action_name)
        if len(overloads) == 0:
            print("No known action named {}".format(action_name))
            return

        overload = self.resolve_overload(
            action_name, len(args), include_simulations_init
        )

        if overload is None:
            print(
                "No known action named {} with {} arguments".format(
                    action_name, len(args)
                )
            )
            return

        return overload.invoke(*args)

    def start(self, args) -> State:
        return State(self, self.execute(args, True))

    def resolve_overload(
        self, overload_name, args_count, include_simulations_init=False
    ):
        for overload in self.get_overloads_of_action(overload_name):
            if not include_simulations_init and overload.is_simulation_init():
                continue

            if len(overload.args) == args_count:
                return overload
        return None


def compile(source, rlc_compiler="rlc", rlc_includes=[]):
    include_args = []
    for arg in rlc_includes:
        include_args.append("-i")
        include_args.append(arg)
    with TemporaryDirectory() as tmp_dir:
        assert (
            run(
                [
                    rlc_compiler,
                    source,
                    "--python",
                    "-o",
                    "{}/wrapper.py".format(tmp_dir),
                ] + include_args
            ).returncode
            == 0
        )
        assert (
            run(
                [rlc_compiler, source, "--shared", "-o", "{}/lib.so".format(tmp_dir)] + include_args
            ).returncode
            == 0
        )
        return Simulation(tmp_dir + "/wrapper.py")
