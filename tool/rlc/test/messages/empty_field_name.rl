# RUN: rlc --expect-fail --print-ir-on-failure=false %s -o %t -i %stdlib 2>&1 | FileCheck %s

# CHECK: 5:18: error: Unexpected token: 'Newline', expected 'Identifier'  
ent Declared:
  NonExistingType

fun main() -> Int:
  let decl : Declared
  return decl.field 
