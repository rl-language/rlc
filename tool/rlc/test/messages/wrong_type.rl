# RUN: rlc --expect-fail --print-ir-on-failure=false %s -o %t -i %stdlib 2>&1 | FileCheck %s

# CHECK: 5:1: error: No known type named NonExistingType used in field field in entity declaration.
# CHECK-NEXT: ent Declared:
ent Declared:
  NonExistingType field

fun main() -> Int:
  let decl : Declared
  return decl.field 
