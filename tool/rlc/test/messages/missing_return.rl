# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 5:1: error: Non void function requires to be terminated by a return statement

fun ret_x(Int vial) -> ref Int:
  vial = 4
