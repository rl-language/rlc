# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
import range

fun main() -> Int:
  let count = 0
  for x in range(3):
    for y in range(3):
      count = count + 1

  return count - 9

