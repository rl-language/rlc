# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  let counter = 4
  return [counter][0] - 4

