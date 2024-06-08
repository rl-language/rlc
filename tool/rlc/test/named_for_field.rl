# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t | FileCheck %s

import serialization.print

# CHECK: first 
# CHECK-NEXT: second
cls Asd:
    Int first
    Int second

fun main() -> Int:
    let x : Asd
    for name, value of x:
        print(name)
    return 0
