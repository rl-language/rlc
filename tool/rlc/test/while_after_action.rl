# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
act play() -> Game:

    act f()

    while true: 
        1

fun main() -> Int:
    let x = play()
    return 0
