# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t

import string

fun main() -> Int:
    if to_string(-285.4) == "-285.400000":
        return 0
    return 1
