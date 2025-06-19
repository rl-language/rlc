# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  let a = 5
  a = a + 2
  if a == 7:
    return 0
  return 1

