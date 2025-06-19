# RUN: rlc %s -o %t -i %stdlib --sanitize -O2 -g
# RUN: %t%exeext

import string

fun main() -> Int:
  let x : String
  x.append("hey")
  if x.get(0) != 'h':
    return -1
  if x.get(1) != 'e':
    return -2
  if x.get(2) != 'y':
    return -3
  if x.size() == 3:
    return 0
  return 1

