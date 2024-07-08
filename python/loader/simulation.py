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
from importlib import import_module, machinery, util
import os
import inspect
from math import log2, ceil, floor
from pathlib import Path
from collections import defaultdict
from tempfile import mkdtemp
from subprocess import run
from ctypes import Structure, Array
from sys import stdout, stderr

loaded_libs = {}

def import_file(name, file_path):
    if name in loaded_libs:
        return loaded_libs[name]
    loader = machinery.SourceFileLoader(name, file_path)
    spec = util.spec_from_loader(name, loader)
    mod = util.module_from_spec(spec)
    loader.exec_module(mod)
    loaded_libs[name] = mod
    return mod


class Action:
    def __init__(self, action, simulation):
        self.action = action
        self.simulation = simulation
        self.functions = simulation.module.functions

    def __str__(self) -> str:
        return self.simulation.to_python_string(self.functions.to_string(self.action))

    def to_bytes(self) -> bytes:
        result = self.functions.as_byte_vector(self.action)
        real_content = []
        for i in range(getattr(result, "__size")):
            real_content.append(getattr(result, "__data")[i] + 128)
        return bytes(real_content)

    def can_run(self, state) -> bool:
        return self.functions.can_apply(self.action, state.state).value

    def run(self, state):
        self.functions.apply(self.action, state.state)
        return state

    def copy(self):
        return Action(self.state.copy(), self.simulation)


class ActionType:
    def __init__(self, action_type, action_id, simulation):
        self.simulation = simulation
        self.action_type = action_type
        self.action_id = action_id

    def get_arg_min_maxs(self):
        return [(0, 3), (0, 3)]

    def materialize(self, *args):
        empty_action = Action(self.simulation.any_action(), self.simulation)
        current_action = self.action_type()
        for (name, type), value in zip(getattr(current_action, "_fields_"), args):
            setattr(current_action, name, value)

        self.simulation.module.functions.assign(empty_action.action, current_action)
        return empty_action


class State:
    def __init__(self, simulation, state):
        self.simulation = simulation
        self.state = state

    def copy(self):
        return State(self.simulation, self.state.copy())

    def is_done(self) -> bool:
        return self.state.resume_index == -1

    def __str__(self) -> str:
        return self.simulation.to_python_string(
            self.simulation.module.functions.to_string(self.state)
        )

    def from_string(self, string: str) -> bool:
        rl_string = self.simulation.to_rl_string(str)
        return self.simulation.module.functions.from_string(self.state, rl_string)

    @property
    def actions(self) -> [Action]:
        return self.simulation.get_actions()

    def as_byte_vector(self):
        result = self.simulation.module.functions.as_byte_vector(self.state)
        real_content = []
        for i in range(getattr(result, "__size")):
            real_content.append(getattr(result, "__data")[i] + 128)
        return bytes(real_content)

    def from_byte_vector(self, byte_vector):
        vector = self.simulation.module.VectorTint8_tT()
        for byte in byte_vector:
            self.simulation.module.functions.append(vector, byte - 128)
        self.simulation.module.functions.from_byte_vector(self.state, vector)

    def write_binary(self, path: str):
        with open(path, mode="wb") as file:
            file.write(self.as_byte_vector())
            file.flush()

    def load_binary(self, path: str):
        with open(path, mode="rb") as file:
            bytes = file.read()
            self.from_byte_vector(bytes)

    def load(self, path: str) -> bool:
        with open(path, mode="r") as file:
            bytes = file.read()
            return self.from_string(bytes)

    def score(self):
        return self.state.score


class Simulation:
    def __init__(self, wrapper: str, tmp_dir=None):
        self.wrapper_path = wrapper
        self.module = import_file("sim", wrapper)
        self.tmp_dir = tmp_dir

        if "play" in self.module.actionToAnyFunctionType:
            self.any_action = self.module.actionToAnyFunctionType["play"]
            any_action_union = getattr(self.any_action(), "content")
            alternatives = getattr(any_action_union, "_fields_")

            self.actions = [
                ActionType(action_type, i, self)
                for (i, (_, action_type)) in enumerate(alternatives)
            ]

    def get_class(self, name):
        return getattr(self.module, name)

    def get_all_actions(self):
        action = self.module.AnyGameAction()
        actions = self.module.functions.enumerate(action)
        result = []
        for i in range(self.module.functions.size(actions)):
            copy = self.module.AnyGameAction()
            self.module.functions.assign(copy, self.module.functions.get(actions, i).contents)
            result.append(copy)


        return result

    def valid_actions(self, state):
        # Convert NumPy arrays to nested tuples to make them hashable.
        x = []
        for action in self.get_all_actions():
            if self.module.functions.can_apply_impl(action, state):
                x.append(action)
        return x

    def can_apply_action(self, action, state):
        return self.module.functions.can_apply_impl(action, state)


    def start(self, name: str, *args):
        return State(self, getattr(self.module.functions, name)(*args))

    def parse_action(self, action: str) -> Action:
        any_action = self.module.actionToAnyFunctionType["play"]()
        rl_string = self.to_rl_string(action)
        if self.module.functions.from_string(any_action, rl_string):
            return Action(any_action, self)
        else:
            return None

    def parse_actions_from_binary_buffer(self, actions: bytes) -> [list]:
        vector = self._bytes_to_byte_vector(actions)
        any_action = self.module.actionToAnyFunctionType["play"]()
        vec = self.module.functions.parse_actions(any_action, vector)
        to_return = []
        for i in range(getattr(vec, "__size")):
            to_return.append(getattr(vec, "__data")[i])
        return to_return

    def _bytes_to_byte_vector(self, byte_vector: bytes):
        vector = self.module.VectorTint8_tT()
        for byte in byte_vector:
            self.module.functions.append(vector, byte)
        return vector

    @property
    def action_names(self) -> [str]:
        return [name for name in self.module.actions.keys()]

    def dump(self):
        for name in self.action_names:
            print(name)

    def to_rl_string(self, string):
        return self.module.rl_s__strlit_r_String(string)

    def to_python_string(self, string):
        first_character = getattr(getattr(string, "__data"), "__data")
        return self.module.cast(first_character, self.module.c_char_p).value.decode(
            "utf-8"
        )

    def action_from_string(self, string: str) -> Action:
        rl_string = self.to_rl_string(str)
        action = self.any_action()
        if not self.module.functions.from_string(action, rl_string):
            return None
        return Action(action, self)

    def action_from_byte_vector(self, byte_vector) -> Action:
        action = self.any_action()
        vector = self.module.VectorTint8_tT()
        for byte in byte_vector:
            self.module.functions.append(vector, byte - 128)
        if not self.module.functions.parse_action_optimized(action, vector, 0):
            return None
        return Action(action, self)

    def __enter__(self):
        return self

    def cleanup(self):
        self.__exit__()

    def __exit__(self, *args):
        import ctypes
        import _ctypes
        libHandle = self.module.lib._handle


    @property
    def functions(self):
        return self.module.functions


def compile(source, rlc_compiler="rlc", rlc_includes=[], rlc_runtime_lib="", optimized=True):
    include_args = []
    for arg in rlc_includes:
        include_args.append("-i")
        include_args.append(arg)
    tmp_dir = mkdtemp() 
    assert (
        run(
            [
                rlc_compiler,
                source,
                "--python",
                "-o",
                Path(tmp_dir) / Path("wrapper.py"),
                "-O2" if optimized else "",
            ]
            + include_args
        ).returncode
        == 0
    )
    lib_name = "lib.dll" if os.name == "nt" else "lib.so"
    args = [rlc_compiler, source, "--shared", "-o", Path(tmp_dir) / Path(lib_name), "-O2" if optimized else ""]
    if rlc_runtime_lib != "":
        args = args + ["--runtime-lib", rlc_runtime_lib]
    assert run(args + include_args).returncode == 0
    return Simulation(str(Path(tmp_dir) / Path("wrapper.py")), tmp_dir)
