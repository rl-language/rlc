# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act test() -> Test:
  frm x = 0
  if x == 0:
    act asd(Int y)
  x = 1

fun main() -> Int:
  let frame = test()
  frame.asd(1) 
  return frame.x - 1
