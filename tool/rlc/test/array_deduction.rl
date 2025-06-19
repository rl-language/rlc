# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun<X, Int T> f(X[T] x) -> X:
  let to_return = x[0]
  if x is Int[T]:
    if to_return is Int:
      to_return = to_return + T
  return to_return

fun main() -> Int:
  let x : Int[4]
  x[0] = 5
  return f(x) - 9

