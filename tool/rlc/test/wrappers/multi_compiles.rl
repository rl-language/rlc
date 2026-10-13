# RUN: split-file %s %t
# RUN: python %t/to_run.py

#--- first.rl

fun first():
    return

#--- second.rl

fun second():
    return

#--- to_run.py
from rlc import compile
import os

dir = os.path.dirname(os.path.realpath(__file__))
program = compile([dir + "/first.rl"], gen_python_methods=False)
print(program.module.first())
program = compile([dir + "/second.rl"], gen_python_methods=False)
print(program.module.second())
