# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 5:1: error: Only types in action functions can be marked as ctx or frm.

fun asd(frm Int x) -> Int:
    return 0
