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
config.substitutions.append(('%pyscript', config.python_scripts))
config.substitutions.append(('%fuzzer_lib', config.rlc_fuzzer_lib))
config.substitutions.append(("%runtime_lib", config.rlc_runtime_lib))
config.substitutions.append(("%pyrlc_lib", config.rlc_pyrlc_lib))
config.substitutions.append(("%exeext", config.llvm_exe_ext))
config.substitutions.append(("%libext", config.llvm_lib_ext))
config.substitutions.append(("%sharedext", config.llvm_shared_ext))
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
config.environment["RLC_PYTHON_LIB_PATH"] = config.rlc_pyrlc_lib
config.environment["RLC_RUNTIME_LIB_PATH"] = config.rlc_runtime_lib
