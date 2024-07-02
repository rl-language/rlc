# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 4:1: error: Non void function requires to be terminated by a return statement
fun main() -> Int:
    if false:
        return 0
