# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import string
import serialization.print

act asd() -> Name:
    act veeeeeereey_looooong()

fun main() -> Int:
    let x : AnyNameAction
    print(x)
    if to_string(x) == "veeeeeereey_looooong {}":
        return 0
    return 1
