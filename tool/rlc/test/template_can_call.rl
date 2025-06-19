# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun<T> function(Int x, T dc) -> Int { x < 4 }:
  return x - 1

fun<T> function2(Int x, T dc) -> Bool:
  return can function(x, dc)

fun main() -> Int:
  if function2(5, 5.0):
    return -4
  else if function2(1, 5.0):
    return 0
  else:
    return 4

