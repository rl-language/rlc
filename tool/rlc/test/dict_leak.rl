# RUN: rlc %s -i %stdlib -o %t --sanitize
# RUN: %t%exeext
import collections.dictionary

act asd() -> Move:
  let dictionary : Dict<Int, Int>
  dictionary.insert(1, 1)
  act inner()

fun main() -> Int:
  asd()
  return 0

