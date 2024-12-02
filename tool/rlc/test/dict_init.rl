# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import serialization.print
import serialization.to_hash
import none

fun main() -> Int:
    let x: Dict<Int,Int>

    let result : Vector<Int>
    x.insert(4,0)
    x.insert(7,0)

    return 0
