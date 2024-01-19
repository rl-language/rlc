# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 6:15: error: Initializer list cannot be empty 

fun main() -> Int:
    let asd = []
    return 0
