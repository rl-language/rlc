# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

import collections.dictionary
import collections.vector
import serialization.print
import serialization.to_hash
import none

cls Point:
  Int x
  Int y

fun main() -> Int:
  let x : Dict<Point, Int>
  let p1 : Point
  p1.x = 1
  p1.y = 2

  x.insert(p1, 42)

  let p2 : Point
  p2.x = 1
  p2.y = 2

  if x.contains(p2) and x.get(p2) == 42:
    return 0
  return 1

