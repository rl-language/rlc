import os
import sys

import lit.util

from lit.llvm import llvm_config
from lit.llvm.subst import ToolSubst
from lit.llvm.subst import FindTool

config.name = 'RLC'

config.suffixes = [
    ".rl",
    ".ll",
]

config.test_format = lit.formats.ShTest(not llvm_config.use_lit_shell)
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.rlc_obj_root, 'test')
lit.util.usePlatformSdkOnDarwin(config, lit_config)
config.substitutions.append(('%PATH%', config.environment['PATH']))
config.substitutions.append(('%shlibext', config.llvm_shlib_ext))
config.substitutions.append(('%stdlib', config.rlc_stdlib))
llvm_config.with_system_environment(["HOME", "INCLUDE", "LIB", "TMP", "TEMP"], append_path=True)
llvm_config.use_default_substitutions()

config.excludes = [
    "CMakeLists.txt",
    "lit.cfg.py",
    "lit.site.cfg.py"
]

llvm_config.with_environment("PATH", config.rlc_tool_dir, append_path=True)
llvm_config.with_environment("PATH", config.rlc_opt_tool_dir, append_path=True)
llvm_config.with_environment("PATH", config.llvm_tools_dir, append_path=True)

tool_dirs = [
    config.rlc_tool_dir,
    config.rlc_opt_tool_dir,
    config.llvm_tools_dir
]

tools = [
    "clang",
    "rlc",
    "rlc-opt"
]

llvm_config.add_tool_substitutions(tools, tool_dirs)

# Set the LD_LIBRARY_PATH
ld_library_path = os.path.pathsep.join((
    config.environment.get("LD_LIBRARY_PATH", "")))

config.environment["LD_LIBRARY_PATH"] = ld_library_path
