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
from types import ModuleType
from shutil import which
from importlib import import_module, machinery, util
import os
import sys
import inspect
from math import log2, ceil, floor
import pathlib
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


class State:
    def __init__(self, program):
        self.program = program
        action = self.module.AnyGameAction()
        self._actions = self.program.functions.enumerate(action)
        self.actions = []
        for i in range(self.program.functions.size(self._actions)):
            self.actions.append(self.program.functions.get(self._actions, i).contents)

        self.num_actions = len(self.actions)

        self.state = self.program.functions.play()

    @property
    def module(self):
        return self.program.module

    @property
    def legal_actions_indicies(self):
        x = []
        for i, action in enumerate(self.actions):
            if self.program.functions.can_apply_impl(action, self.state).value:
                x.append(i)
        return x

    @property
    def legal_actions(self):
        x = []
        for action in self.actions:
            if self.program.functions.can_apply_impl(action, self.state).value:
                x.append(action)
        return x

    def reset(self, seed=None, options=None, path_to_binary_state=None):
        if path_to_binary_state == None:
            self.state = self.program.functions.play()
        else:
            self.load_binary(path_to_binary_state)

    def step(self, action):
        assert self.can_apply(action)
        self.program.functions.apply(action, self.state)

    def can_apply(self, action) -> bool:
        return self.program.functions.can_apply(action, self.state)

    def as_byte_vector(self):
        return self.program.as_byte_vector(obj, self.state)

    def load_string(self, string: str) -> bool:
        return self.program.load_string(string, self.state)

    def load_byte_vector(self, byte_vector):
        self.program.load_byte_vector(byte_vector, self.state)

    def write_binary(self, path: str):
        self.program.write_binary(path, self.state)

    def load_byte_vector_from_file(self, path: str):
        self.program.load_string_from_file(path, self.state)

    def load_string_from_file(self, path: str) -> bool:
        self.program.load_string_from_file(path, self.state)

    def print(self):
        program.functions.print(state.state)

    def is_done(self) -> bool:
        return self.state.resume_index == -1

    def to_string(self) -> str:
        return self.program.to_string(self.state)

    def print(self) -> str:
        return self.program.functions.print(self.state)

    def pretty_print(self) -> str:
        return self.program.functions.pretty_print(self.state)


class Program:
    def __init__(self, module_path: str, tmp_dir=None):
        if not isinstance(module_path, ModuleType):
            self.module_path = module_path
            self.module = import_file("sim", module_path)
        else:
            self.module = module_path
            self.module_path = None
        self.tmp_dir = tmp_dir

    def start(self) -> State:
        return State(self)

    def parse_action(self, action: str):
        any_action = self.module.actionToAnyFunctionType["play"]()
        rl_string = self.to_rl_string(action)
        if self.module.functions.from_string(any_action, rl_string):
            return any_action
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

    def to_string(self, action):
        return self.to_python_string(self.module.functions.to_string(action))

    def as_byte_vector(self, obj):
        result = self.module.functions.as_byte_vector(obj)
        real_content = []
        for i in range(getattr(result, "__size")):
            real_content.append(getattr(result, "__data")[i] + 128)
        return bytes(real_content)

    def load_string(self, string: str, obj) -> bool:
        rl_string = self.to_rl_string(string)
        return self.module.functions.from_string(obj, rl_string)

    def load_byte_vector(self, byte_vector, obj):
        vector = self.module.VectorTint8_tT()
        for byte in byte_vector:
            self.module.functions.append(vector, byte - 128)
        self.module.functions.from_byte_vector(obj, vector)

    def write_binary(self, path: str, obj):
        with open(path, mode="wb") as file:
            file.write(self.as_byte_vector(obj))
            file.flush()

    def load_byte_vector_from_file(self, path: str):
        with open(path, mode="rb") as file:
            bytes = file.read()
            self.from_byte_vector(bytes)

    def load_string_from_file(self, path: str, obj) -> bool:
        with open(path, mode="r") as file:
            bytes = file.read()
            return self.load_string(bytes, obj)


def stdlib_location(rlc):
    rlc_path = which(rlc)
    assert (
        rlc_path is not None
    ), "could not find executable {}, use --rlc <path_to_rlc> to configure it".format(
        rlc
    )
    return pathlib.Path(rlc_path).parent.parent.absolute() / "lib" / "rlc" / "stdlib"


def stdlib_file(file, rlc="rlc", stdlib=None):
    if stdlib != None:
        return pathlib.Path(stdlib) / file
    return stdlib_location(rlc) / file


def compile(
    sources=[],
    rlc_compiler="rlc",
    rlc_includes=[],
    rlc_runtime_lib="",
    optimized=True,
    gen_python_methods=True,
    stdlib=None,
) -> Program:
    s = [source for source in sources]
    if gen_python_methods:
        s.append(stdlib_file("learn.rl", rlc_compiler, stdlib=stdlib))

    include_args = []
    for arg in rlc_includes:
        include_args.append("-i")
        include_args.append(arg)
    if stdlib != None:
        include_args.append("-i")
        include_args.append(stdlib)
    tmp_dir = mkdtemp()

    assert (
        run(
            [
                rlc_compiler,
                *s,
                "--python",
                "-o",
                Path(tmp_dir) / Path("wrapper.py"),
                "-O2" if optimized else "",
            ]
            + include_args
        ).returncode
        == 0
    )
    lib_name = "lib.dll" if os.name == "nt" else ("lib.dylib" if sys.platform == "darwin" else "lib.so")
    args = [
        rlc_compiler,
        *s,
        "--shared",
        "-o",
        Path(tmp_dir) / Path(lib_name),
        "-O2" if optimized else "",
    ]
    if rlc_runtime_lib != "":
        args = args + ["--runtime-lib", rlc_runtime_lib]
    assert run(args + include_args).returncode == 0
    return Program(str(Path(tmp_dir) / Path("wrapper.py")), tmp_dir)
