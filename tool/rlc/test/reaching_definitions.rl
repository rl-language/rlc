# RUN: rlc %s -o %t -i %stdlib --sanitize -g
# RUN: %t%exeext

import collections.vector

cls Asd:
  Vector<Int> a

  fun f() -> Asd:
    let other : Asd
    return other

fun make_asd() -> Asd:
  let other : Asd
  return other

fun main() -> Int:
  make_asd().f()
  return 0

