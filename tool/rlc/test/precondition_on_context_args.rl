# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls EntOne:
  Bool field_one

cls EntTwo:
  Bool field_two

act play(ctx EntOne one, ctx EntTwo two) -> Play:
  one.field_one
  act subact() { one.field_one }

fun main() -> Int:
  return 0

