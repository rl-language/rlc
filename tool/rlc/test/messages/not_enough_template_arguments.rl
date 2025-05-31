# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: :10:12: error: Could not find matching function add(Int,Int)

fun<T> add(Int a, Int b) -> Int:
    return a + b


fun main() -> Int:
    return add(5, 3) - 8 
