# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import serialization.print
import string

@classes
act to_run() -> ToRun:
    act first(Bool asd)
    act second(Int tasd)

fun main() -> Int:
    let x : ToRunFirst | ToRunSecond
    print(to_string(x))
    if to_string(x) == "first {asd: false}":
        return 0
    return 1
