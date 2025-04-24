# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

const x = -1

fun main() -> Int:
    return x + 1

