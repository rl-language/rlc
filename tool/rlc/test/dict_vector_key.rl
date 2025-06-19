# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import collections.vector
import serialization.print
import serialization.to_hash
import none

fun main() -> Int:
  let x : Dict<Vector<Int>, Int>
  let v : Vector<Int>
  v.append(1)
  v.append(2)

  x.insert(v, 42)

  let v2 : Vector<Int>
  v2.append(1)
  v2.append(2)

  if x.contains(v2) and x.get(v2) == 42:
    return 0
  return 1

