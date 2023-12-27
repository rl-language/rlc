# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act inner(Int arg) -> Inner:
    arg = 2
    act set_to_5(Int arg)
    arg = 5
    act set_to_anything(Int arg, Int val)
    arg = val

act outer() -> Outer:
    let result : Int
    subaction*(result) inner_frame = inner(result)

fun main() -> Int:
    let frame = outer()
    if frame.result != 2:
        return -1
    frame.set_to_5()
    if frame.result != 5:
        return -2
    frame.set_to_anything(10)
    if frame.result == 10:
        return 0
    return -3
