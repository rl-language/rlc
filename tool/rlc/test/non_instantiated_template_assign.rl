# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

cls<T> Example:
  T value
  fun assign(Example<T> other):
   self.value = 4

fun asd(Example<Int> arg):
  let x = 0

fun main() -> Int:
  let x : Example<Int>
  let x2 : Example<Int>
  return x.value
