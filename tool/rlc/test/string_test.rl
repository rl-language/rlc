# RUN: rlc %s -o %t -i %stdlib --sanitize -O2 -g
# RUN: %t%exeext

import string

fun main() -> Int:
    let x : String
    return x.size()


