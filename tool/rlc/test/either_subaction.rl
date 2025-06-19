# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act inner() -> Inner:
  act asd()

act inner2() -> Inner2:
  act asd2()

act outer() -> Outer:
  frm first = inner()
  frm second = inner2()
  subaction* first, second

fun main() -> Int:
  let state = outer()
  if state.is_done():
    return 1
  state.asd()
  if state.is_done():
    return 1
  state.asd2()
  if state.is_done():
    return 0
  return 1

