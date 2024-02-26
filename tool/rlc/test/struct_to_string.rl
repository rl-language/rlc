# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t

import string
import collections.vector
import serialization.print

ent SomeStruct:
    Bool x
    Vector<Int> y

fun main() -> Int:
    let var : SomeStruct
    var.x = true
    var.y.append(3)
    var.y.append(4)
    if to_string(var) == "{true, [3, 4]}":
        return 0
    return -1
