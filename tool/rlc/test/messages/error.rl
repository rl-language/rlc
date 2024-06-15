# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 5:1: error: Action type cannot be primitive type 

act asd() -> Int:
    let a = 0 

fun main() -> Int:
    return 0
