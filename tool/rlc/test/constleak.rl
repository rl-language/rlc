# RUN: rlc %s -i %stdlib -o %t --sanitize
# RUN: %t%exeext
import collections.vector

const LIMIT = 13

fun leaking() -> Bool:
  let values : Vector<Int>
  while values.size() < LIMIT:
    values.append(1)
  return values.size() == 13

fun main() -> Int:
  return int(!leaking())

