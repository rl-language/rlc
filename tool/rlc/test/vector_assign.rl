# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls Asd:
  Int value

fun main() -> Int:
  let x : Asd[10]
  let y : Asd[10]
  x[3].value = 1
  y = x
  return y[3].value - 1

