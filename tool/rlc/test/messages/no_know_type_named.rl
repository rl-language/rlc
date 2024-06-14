# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 8:18: error: No known type named Vectora

import collections.vector

fun main() -> Int:
    let vector : Vectora<Int, Int>
    return 0
