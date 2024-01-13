# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 6:5: error: Action statements can only appear in Action Functions

fun function(Int x):
    act play()

