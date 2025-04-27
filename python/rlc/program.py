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
        self._actions = self.program.module.enumerate(action)
        self.actions = []
        for i in range(self._actions.size()):
            self.actions.append(self._actions.get(i).contents)

        self.num_actions = len(self.actions)

        self.state = self.program.module.play()

    @property
    def raw_actions(self):
        return self._actions

    @property
    def module(self):
        return self.program.module

    @property
    def legal_actions_indicies(self):
        x = []
        for i, action in enumerate(self.actions):
            if self.program.module.can_apply_impl(action, self.state):
                x.append(i)
        return x

    @property
    def legal_actions(self):
        x = []
        for action in self.actions:
            if self.program.module.can_apply_impl(action, self.state):
                x.append(action)
        return x

    def reset(self, seed=None, options=None, path_to_binary_state=None):
        if path_to_binary_state == None:
            self.state = self.program.module.play()
        else:
            self.load_binary(path_to_binary_state)

    def step(self, action):
        if not self.can_apply(action):
            self.module.print(action)
            sys.stdout.flush()
            assert (
                len(self.legal_actions) != 0
            ), "found a state with no valid actions, yet the game is not terminated"
            return
        self.program.module.apply(action, self.state)

    def can_apply(self, action) -> bool:
        return self.program.module.can_apply(action, self.state)

    def as_byte_vector(self):
        return self.program.module.as_byte_vector(obj, self.state)

    def load_string(self, string: str) -> bool:
        return self.program.module.load_string(string, self.state)

    def load_byte_vector(self, byte_vector):
        self.program.module.load_byte_vector(byte_vector, self.state)

    def write_binary(self, path: str):
        self.program.module.write_binary(path, self.state)

    def load_byte_vector_from_file(self, path: str):
        self.program.module.load_string_from_file(path, self.state)

    def load_string_from_file(self, path: str) -> bool:
        return self.program.module.load_string_from_file(path, self.state)

    def print(self):
        self.program.module.print(self.state)

    def is_done(self) -> bool:
        return self.state.resume_index == -1

    def __str__(self) -> str:
        return str(self.state)

    def pretty_print(self) -> str:
        return self.program.module.pretty_print(self.state)


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
        if self.module.from_string(any_action, rl_string):
            return any_action
        else:
            return None

    def parse_actions_from_binary_buffer(self, actions: bytes) -> [list]:
        vector = self._bytes_to_byte_vector(actions)
        any_action = self.module.actionToAnyFunctionType["play"]()
        vec = self.module.parse_actions(any_action, vector)
        to_return = []
        for i in range(getattr(vec, "_size")):
            to_return.append(getattr(vec, "_data")[i])
        return to_return

    def _bytes_to_byte_vector(self, byte_vector: bytes):
        vector = self.module.VectorTint8_tT()
        for byte in byte_vector:
            vector.append(byte)
        return vector

    @property
    def action_names(self) -> [str]:
        return [name for name in self.module.actions.keys()]

    def dump(self):
        for name in self.action_names:
            print(name)

    def to_rl_string(self, string):
        return self.module.rl_s__strlit_r_String(string)

    def __enter__(self):
        return self

    def cleanup(self):
        self.__exit__()

    def __exit__(self, *args):
        import ctypes
        import _ctypes

        libHandle = self.module.lib._handle

    def as_byte_vector(self, obj):
        result = self.module.as_byte_vector(obj)
        real_content = []
        for i in range(getattr(result, "_size")):
            real_content.append(getattr(result, "_data")[i] + 128)
        return bytes(real_content)

    def load_string(self, string: str, obj) -> bool:
        rl_string = self.to_rl_string(string)
        return self.module.from_string(obj, rl_string)

    def load_byte_vector(self, byte_vector, obj):
        vector = self.module.VectorTint8_tT()
        for byte in byte_vector:
            vector.append(byte - 128)
        self.module.from_byte_vector(obj, vector)

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


def get_included_contents(
    rl_file: str,
    exclude_stdlib: bool = True,
    rlc_compiler="rlc",
    rlc_includes=[],
    stdlib=None,
) -> str:
    # Returns the content of the provided rl file,
    # and the contents of each recursivelly imported file.
    # Returns None if the program could not parse. The content of the file otherwise.
    include_args = []
    for arg in rlc_includes:
        include_args.append("-i")
        include_args.append(arg)
    if stdlib != None:
        include_args.append("-i")
        include_args.append(stdlib)
    result = run(
        [
            rlc_compiler,
            "--print-included-files",
            f"--hide-standard-lib-files={exclude_stdlib}",
            rl_file,
        ]
        + include_args,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(result.stdout)
        return None
    return result.stdout


def compile(
    sources=[],
    rlc_compiler="rlc",
    rlc_includes=[],
    rlc_runtime_lib="",
    pyrlc_runtime_lib=None,
    optimized=True,
    gen_python_methods=True,
    stdlib=None,
    extra_rlc_args=[],
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
            + extra_rlc_args
        ).returncode
        == 0
    )
    lib_name = (
        "lib.dll"
        if os.name == "nt"
        else ("lib.dylib" if sys.platform == "darwin" else "lib.so")
    )
    args = [
        rlc_compiler,
        *s,
        "--shared",
        "--pylib",
        "-o",
        Path(tmp_dir) / Path(lib_name),
        "-O2" if optimized else "",
    ]
    if rlc_runtime_lib != "":
        args = args + ["--runtime-lib", rlc_runtime_lib]
    if pyrlc_runtime_lib != None:
        args = args + ["--pyrlc-lib", pyrlc_runtime_lib]
    assert run(args + include_args).returncode == 0
    return Program(str(Path(tmp_dir) / Path("wrapper.py")), tmp_dir)
