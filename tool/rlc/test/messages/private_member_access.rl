# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t3 -i %stdlib -i %t --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 3:4: error: Members starting with _ are private and cannot be accessed from another module

#--- source.rl
import to_include

ent Asd:
  Int _x

#--- to_include.rl
fun function():
  let y : Asd
  y._x

