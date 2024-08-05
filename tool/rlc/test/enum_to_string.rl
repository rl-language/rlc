# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import string

enum Example:
    first
    second

fun main() -> Int:
    if "first"s != as_string_literal(Example::first):
        return -1
    if "second"s != as_string_literal(Example::second):
        return -2
    return 0
