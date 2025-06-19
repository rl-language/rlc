# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

@classes
act asd(ctx Int arg) -> Inner:
  frm copy = arg
  act x()
  copy = copy + arg

fun main() -> Int:
  let frame = asd(3)
  let action : AnyInnerAction
  apply(action, frame, 2)
  return frame.copy - 5

