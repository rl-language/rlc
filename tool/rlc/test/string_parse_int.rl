# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import string

fun main() -> Int:
    if to_string(-285) == "-285":
        return 0
    return 1
