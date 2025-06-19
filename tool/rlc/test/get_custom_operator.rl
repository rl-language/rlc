# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import collections.vector

fun main() -> Int:
  let t : Vector<Int>
  t.append(2)
  return t[0] - 2

