# RUN: rlc %s -i %stdlib -o %t --sanitize
# RUN: %t%exeext
import collections.vector
import serialization.print

fun main() -> Int:
  while true:
    let vector : Vector<Int>
    if 4 == 4:
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      vector.append(1)
      break
    else:
      vector.append(1)

  return 0

