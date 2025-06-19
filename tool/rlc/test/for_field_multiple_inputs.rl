# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls Asd:
  Int v1
  Int v2

fun<T> cross_product(T x, T y) -> Int:
  let sum = 0
  for f1, f2 of x, y:
    if f1 is Int:
      if f2 is Int:
        sum = sum + (f1 * f2)

  return sum

fun main() -> Int:
  let x : Asd
  let y : Asd
  x.v1 = 10
  y.v1 = 3
  x.v2 = 2
  y.v2 = 4

  return cross_product(x, y) - 38

