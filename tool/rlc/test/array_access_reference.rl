# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
    let x : Int[4]
    x[2] = 10
    ref reference = x[2]
    reference = 2
    return x[2] - 2
