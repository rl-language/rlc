# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import none

fun main() -> Int:
  let x : Dict<Int, Int>
  x.insert(1, 1)
  x.insert(2, 2)
  x.clear()

  if x.size() == 0:
    return 0
  else:
    return 1

