# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import string 

act play() -> Name:
    act first()
    act second(Int asd)

fun main() -> Int:
    let any_action : AnyNameAction
    let second_action : NameSecond
    any_action = second_action
    if to_string(any_action) == "second {asd: 0}":
        return 0
    return 1
