# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import serialization.print
import serialization.to_hash
import none

fun main() -> Int:
    let x: Dict<Int,Int>

    let result : Vector<Int>
    print(compute_hash_of(4))
    x.insert(4,0)
    let counter = 1
    let max = 0

    return 0
