# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
import bounded_arg
import serialization.print

fun main() -> Int:
  let x : BInt<0, 3>
  x.value = 2
  if to_string(x) != "2"s:
    return -1

  let loaded : BInt<0, 3>
  from_string(loaded, "2"s)
  if loaded != x:
    return -2

  let y = byte(-127)
  let out = as_byte_vector(y)

  from_byte_vector(loaded, out)

  if loaded.value != 1:
    return -2

  if as_byte_vector(loaded).size() != 1:
    return -3

  return 0

