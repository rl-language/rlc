# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
    let a = 0	
    let b = a
    b = b + 1
    return a
