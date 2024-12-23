# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

@classes
act action() -> Action:
    frm to_return = 0
    act to_call(Int arg)
        to_return = arg


fun main() -> Int:
    let x : ActionToCall
    if x is ActionAction:
        return 0
    else:
        return 1

