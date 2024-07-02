# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act action() -> Action:
    frm to_return = 0
    act to_call(Int arg)
        to_return = arg

fun main() -> Int:
    let frame = action()
    let x : ActionToCall
    x.arg = 4

    apply(x, frame)
    
    if frame.to_return == 4:
        return 0
    else:
        return 1
