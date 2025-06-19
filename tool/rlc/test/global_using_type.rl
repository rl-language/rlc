# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import collections.vector

using T = Int | Vector<Int>

fun main() -> Int:
  let asd : T
  asd = 10
  if asd is Int:
    return asd - 10
  return -10

