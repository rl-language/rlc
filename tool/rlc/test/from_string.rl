# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import string
import collections.vector

cls Asd:
    Vector<Int> asd
    Float | Bool tasd

fun main() -> Int:
    let result : Asd
    if !from_string(result, "{asd: [2, 3], tasd: Bool true}"s):
        return 2
    if result.asd.size() != 2:
        return 1
    if result.asd.get(0) != 2:
        return 1
    if result.asd.get(1) != 3:
        return 1
    return 0
