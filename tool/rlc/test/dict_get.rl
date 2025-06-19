# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import none

fun main() -> Int:
  let x : Dict<Int, Int>
  x.insert(1, 2)

  if x.get(1) == 2:
    return 0
  else:
    return 1

