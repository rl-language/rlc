# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 14:7: error: Could not find matching function assign(A,B)

cls A:
    Int b

cls B:
    Int b

fun asd(): 
    let a : A
    let b : B
    a = b
