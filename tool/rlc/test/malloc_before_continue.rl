# RUN: rlc %s -i %stdlib -o %t --sanitize
# RUN: %t%exeext
import collections.vector
import serialization.print

fun main() -> Int:
  let a = 3
  while 4 == a:
    let vector : Vector<Int>
    if a == 3:
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      a = a + 1
      print(vector)
      continue
    else:
      vector.append(1)

  return 0

