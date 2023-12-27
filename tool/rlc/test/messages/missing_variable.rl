# RUN: rlc --expect-fail --print-ir-on-failure=false %s -o %t -i %stdlib 2>&1 | FileCheck %s

# CHECK: 7:10: error: No known value x2
# CHECK-NEXT: return x2
fun main() -> Int:
  let x1 = 5
  return x2
