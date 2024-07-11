# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 7:7: error: Cannot assign Float to Int. Cast it instead

fun asd(): 
    let a = 0 
    a = 0.0

fun main() -> Int:
    return 0
