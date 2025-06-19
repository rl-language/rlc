# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls<T> Class:
  T _field

  fun get_field() -> T:
    return self._field

  fun set_field(T x):
    self._field = x

  fun<R> identity(R val) -> R:
    return val

fun main() -> Int:
  let x : Class<Int>
  x.set_field(4)
  return x.identity(x.get_field() - 4)

