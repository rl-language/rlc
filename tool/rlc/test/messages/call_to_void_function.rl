# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 9:14: error: Call of void returning function cannot be used as expression

fun call(Int val):
    val = val + 1

fun main() -> Int:
    call(call(5))

