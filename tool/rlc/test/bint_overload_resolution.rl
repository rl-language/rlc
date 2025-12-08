# RUN: rlc %s -o %t -i %stdlib --abort-symbol=abort
# RUN: %t%exeext
cls<Int min, Int max> BInt:
  Int x

fun<Int min, Int max> my_function(BInt<min, max> x) -> Int:
  return max - min

fun<T> my_function(T x) -> Int:
  assert(false, "should not be called")
  return 0

fun main() -> Int:
  let x : BInt<0, 9>
  my_function(x)
  return 0

