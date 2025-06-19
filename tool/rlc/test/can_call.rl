# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun function(Int x) -> Int { x < 4 }:
  return x - 1

fun main() -> Int:
  if can function(5):
    return -4
  else if can function(1):
    return 0
  else:
    return 4

