# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import string

fun main() -> Int:
  let x = false
  let y = true
  from_string(y, to_string(x))
  if x == y:
    return 0
  return -1

