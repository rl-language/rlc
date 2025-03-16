# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import collections.vector
import serialization.print
import serialization.to_hash
import none

fun main() -> Int:
    let x: Dict<Int,Int>
    x.insert(1, 1)
    x.remove(1)

    if x.empty():
        return 0
    else:
        return 1
    
