# RUN: rlc %s -o %t -i %stdlib --sanitize 
# RUN: %t%exeext

using Inner = Int | Float
using Inner2 = Int | Float
using Outer = Inner2 | Inner

fun main() -> Int:
    let x : Outer
    if x is Inner2:
        return 0
    return 1
