# RUN: rlc %s -o %t -i %stdlib --sanitize -O2 -g
# RUN: %t%exeext

import string

fun main() -> Int:
    let x = "hey"s + "wey"s
    if x == "heywey"s:
        return 0
    return 1

