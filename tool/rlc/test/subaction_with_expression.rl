# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act with_context(ctx Int a) -> Type:
  act do_something()

cls G:
  Int a

act caller() -> Type2:
  frm g : G
  subaction*(g.a) sub = with_context(g.a)

fun main() -> Int:
  let c = caller()
  c.do_something()
  if c.is_done():
    return 0
  return 1

