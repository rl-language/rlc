# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import string

fun main() -> Int:
    let x : Int[2]
    let y : Int[2]
    x[0] = 1
    y[1] = 1
    from_string(y, to_string(x))
    if x[0] == y[0] and x[1] == y[1]: 
        return 0
    return -1
