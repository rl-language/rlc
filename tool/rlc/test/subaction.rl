# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act outer() -> Outer:
  frm frame = inner()
  act run(Int x)
  frame.asd(x)

act inner() -> Inner:
  frm to_return : Int
  act asd(Int x)
  to_return = x

fun main() -> Int:
  let frame = outer()
  frame.run(8)
  return frame.frame.to_return - 8

