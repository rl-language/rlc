act inner(Int local_var) -> Inner:
  act to_call(Int other){
    other == local_var + 1
  }
  local_var = other

act outer() -> Outer:
  subaction inner_frame = inner(5)

fun main() -> Int:
  let outer_frame = outer()
  outer_frame.to_call(6)
  return outer_frame.inner_frame.local_var - 6
