# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

const global = 3

cls<Int T> Asd:
  fun get_t() -> Int:
    return T

using Fixed = Asd<global>

fun main() -> Int:
  let x : Fixed
  if x.get_t() == 3:
    return 0
  else:
    return -1

