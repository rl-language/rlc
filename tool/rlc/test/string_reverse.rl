# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import string

fun main() -> Int:
    let str = "hey"s
    str.reverse()
    if str == "yeh"s:
        return 0
    return 1
