# RUN: rlc --expect-fail --print-ir-on-failure=false %s -o %t -i %stdlib 2>&1 | FileCheck %s

# CHECK: 11:10: error: Could not find matching function correct(Int)
# CHECK-NEXT: return correct(4)
# CHECK: 7:1: remark: Candidate: (Float) -> Int
# CHECK-NEXT: fun correct(Float asd) -> Int:
fun correct(Float asd) -> Int:
  return 0

fun main() -> Int:
  return correct(4)
