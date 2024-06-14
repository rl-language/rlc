# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 6:5: error: Break statement cannot be used outside of a while statement 

fun function(Int x):
    break


