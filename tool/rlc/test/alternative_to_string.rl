# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t

import string
import collections.vector
import serialization.print

cls SomeStruct:
    Bool | Float x
    Vector<Int> y

fun main() -> Int:
    let var : SomeStruct
    var.x = true
    var.y.append(3)
    var.y.append(4)
    if to_string(var) == "{x: Bool true, y: [3, 4]}":
        return 0
    return -1

