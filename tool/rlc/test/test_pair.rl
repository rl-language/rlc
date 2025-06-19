# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import collections.pair

fun main() -> Int:
  let first : Vector<Bool>
  let second : Vector<Float>
  first.append(true)
  first.append(false)
  second.append(0.0)
  second.append(1.1)
  let zipped = zip(first, second)
  if zipped.get(0).first != true:
    return -1
  if zipped.get(1).first != false:
    return -1
  if zipped.get(0).second != 0.0:
    return -1
  if zipped.get(1).second != 1.1:
    return -1
  return 0

