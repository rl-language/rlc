# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 10:7: error: Could not find matching function assign(A,Float)

cls A:
    Int b

fun asd(): 
    let a : A
    a = 0.0

