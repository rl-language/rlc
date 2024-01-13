# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 5:1: error: Function with non-void return type needs at least a return statement

fun ret_x(Int vial) -> ref Int:
  vial = 4
