# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import collections.dictionary
import collections.vector
import none

fun main() -> Int:
  let x : Dict<Int, Int>
  x.insert(1, 2)

  let actual : Vector<Int>
  actual = x.values()

  let expected : Vector<Int>
  expected.append(2)

  let i = 0
  while i < actual.size():
    if actual.get(i) != expected.get(i):
      return 1
    i = i + 1

  return 0

