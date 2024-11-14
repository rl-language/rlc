# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
    let x = ~0
    return x + 1
