# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import action

@classes
act play() -> Game:
  frm counter = 0
  while true:
    actions:
      act quit()
      return

      act count(Int x)
      counter = counter + x

fun main() -> Int:
  let serialized : Vector<Byte>

  let action1 : GameCount
  action1.x = 4

  let action2 : GameQuit

  let to_append : AnyGameAction
  to_append = action1
  append_to_byte_vector(to_append, serialized)

  let to_append2 : AnyGameAction
  to_append = action2
  append_to_byte_vector(to_append2, serialized)

  let frame = play()

  let action : AnyGameAction
  parse_and_execute(frame, action, serialized)

  if !frame.is_done():
    return -1
  else if frame.counter != 4:
    return 1

  return 0

