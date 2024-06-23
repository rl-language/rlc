# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import string
import serialization.print

enum Example:
    blue:
        String name = "blu"s
    red:
        String name = "red"s

fun main() -> Int:
    if Example::blue.name() != "blu"s:
        return -10
    if Example::red.name()  == "red"s:
        return 0
    return 1

