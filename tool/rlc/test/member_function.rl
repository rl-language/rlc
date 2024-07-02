# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls Class:
  fun get_field() -> Int:
    return self._field

  fun set_field(Int x):
    self._field = x

  Int _field

fun main() -> Int:
  let x : Class
  x.set_field(4)
  return x.get_field() - 4

