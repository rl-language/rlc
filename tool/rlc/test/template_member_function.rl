# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent<T> Class:
  fun get_field() -> T:
    return self._field

  fun set_field(T x):
    self._field = x

  fun<R> identity(R val) -> R:
    return val

  T _field

fun main() -> Int:
  let x : Class<Int>
  x.set_field(4)
  return x.identity(x.get_field() - 4)

