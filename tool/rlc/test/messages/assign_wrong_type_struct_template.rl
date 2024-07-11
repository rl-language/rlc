# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 14:7: error: Could not find matching function assign(A,B)

cls A:
    Int | Float c

cls B:
    Int b

fun<T> asd(T a): 
    let a : T
    let b : B
    a = b

fun main() -> Int:
    let a : A
    asd(a)
    return 0
