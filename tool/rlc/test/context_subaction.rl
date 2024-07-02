# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act action_with_arg(ctx Int arg) -> Name:
    act inner(Int inner_arg) {arg + 2 == inner_arg}
    arg = arg + 1
    act inner2(Int inner_arg) {arg - 2 == inner_arg}
    arg = arg + 1

act outer_middle(ctx Int context) -> WrapperMiddle:
    subaction* (context) frame = action_with_arg(context)

act outer() -> Wrapper:
    frm context = 3
    subaction* (context) frame = outer_middle(context)

fun main() -> Int:
    let frame = outer()
    frame.inner(5)
    frame.inner2(2)
    return frame.context - 5
