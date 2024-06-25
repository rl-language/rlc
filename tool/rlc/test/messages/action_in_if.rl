# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 6:5: error: Declaration statement declared as a local variable 

act asd() -> Asd:
    let a = 0 
    if a == a:
        act c()
    a = 3

fun main() -> Int:
    return 0
