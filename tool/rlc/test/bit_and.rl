# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import serialization.print

fun main() -> Int:
    let x = 2 & 3
    print(x)
    return x - 2
