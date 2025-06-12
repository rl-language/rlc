# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
import rng
import serialization.print

fun main() -> Int:
    let rng : RNG
    rng.set_seed(10)
    if rng.randint(0, 10) != 1:
        return -1
    if rng.randint(0, 10) != 1:
        return -1
    if rng.randint(0, 10) != 2:
        return -1
    if rng.randint(0, 10) != 2:
        return -1
    if rng.randint(0, 10) != 5:
        return -1
    if rng.randint(0, 10) != 4:
        return -1 
    if rng.randint(0, 10) != 5:
        return -1
    return 0
