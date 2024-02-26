# RUN: rlc %s -o %t -i %stdlib --sanitize -O2 -g
# RUN: %t

import string

fun main() -> Int:
    let x = "hey"s 
    return x.size() - 3
