# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls Example:
  Int f1

  fun init():
    self.f1 = 5

fun main() -> Int:
  let asd : Example
  return asd.f1 - 5

