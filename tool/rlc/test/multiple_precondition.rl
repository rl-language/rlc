# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun f(Int x) -> Int { true, x == 5 }:
    return x + 1

fun main() -> Int:
    if f(5) == 6:
        return 0
    return 1
