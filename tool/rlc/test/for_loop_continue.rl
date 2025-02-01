# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import range

fun main() -> Int:
    let sum = 0
    for i in range(10):
        if i == 5:
           continue 
        sum = sum + i 
    if sum == 40:
        return 0
    return 1
