# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act action_with_arg(ctx Int arg) -> Name:
    act inner(Int inner_arg) {arg + 2 == inner_arg}
    arg = arg + 1

fun main() -> Int:
    let context = 3
    let frame = action_with_arg(context)
    frame.inner(context, 5)
    return context - 4
