# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 8:18: error: Template type has 1 parameters but 2 were provided.

import collections.vector

fun main() -> Int:
    let vector : Vector<Int, Int>
    return 0
