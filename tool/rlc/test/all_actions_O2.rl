# RUN: rlc %s -o %t -i %stdlib -O2
# RUN: %t

import action
import bounded_arg

ent Asd:
  Bool field

fun main() -> Int:
  let x : Asd | Bool
  let state = enumerate(x)
  print(state)
  return 0
