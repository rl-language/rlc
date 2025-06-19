# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import collections.vector
import serialization.print
import serialization.to_hash
import none

fun main() -> Int:
  let x : Dict<Int, Vector<Int>>
  let v : Vector<Int>
  v.append(1)
  v.append(2)

  x.insert(1, v)

  if x.contains(1):
    let result = x.get(1)
    if result.size() == 2 and result.get(0) == 1 and result.get(1) == 2:
      return 0
  return 1

