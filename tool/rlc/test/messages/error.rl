# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 7:1: error: No known type named Inta

import collections.vector

fun main(Inta a) -> Int:
    let vector : Vector<Int>
    return
