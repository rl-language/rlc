# RUN: rlc %s -o %t -i %stdlib
# RUN: %t%exeext

act to_run() -> ToRun:
  frm to_ret : Int
  act dead_store(Int val)
  to_ret = val
  act to_call(Int val2)
  to_ret = val2
  act final(Int val3)
  to_ret = val3

fun frame_creator() -> ToRun:
  let frame = to_run()
  frame.dead_store(1)
  return frame

act outer() -> Outer:
  subaction frame = frame_creator()
  frame.final(10)

fun main() -> Int:
  let frame = outer()
  frame.to_call(4)
  return frame.frame.to_ret - 10

