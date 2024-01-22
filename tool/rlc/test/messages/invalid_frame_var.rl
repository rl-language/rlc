# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 5:1: error: Action function argument declared as a local variable, but it is used in different actions.

act ret_x(Int val) -> Name:
    act asd()
    val = 4

fun main() -> Int:
    let frame = ret_x(4)
    return 0 
