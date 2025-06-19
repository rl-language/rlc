# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext | FileCheck %s

import serialization.print

# CHECK: result is: 4
fun main() -> Int:
  print("result is: "s + to_string(4))
  return 0

