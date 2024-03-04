# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import action

act play() -> Game:
    frm counter = 0
    while true:
        actions:
            act quit()
                return
            act count(Int x)
                counter = counter + x

fun main() -> Int:
    let action_vector : Vector<AnyGameAction>

    let action1 : GameCount
    action1.x = 4

    let action2 : GameQuit

    let to_append : AnyGameAction
    to_append = action1
    action_vector.append(to_append)

    let to_append2 : AnyGameAction
    to_append = action2
    action_vector.append(to_append2)

    let serialized = as_byte_vector(action_vector)

    let frame = play()

    let action : AnyGameAction
    parse_and_execute(frame, action, serialized)

    if !frame.is_done():
        return -1
    else if frame.counter != 0:
        return 1
    return 0
