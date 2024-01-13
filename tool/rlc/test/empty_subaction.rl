# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act inner(Int x) -> Action:
    x = 10

act outer(Int x) -> Action2:
    subaction* frame = inner(x)

fun main() -> Int:
    let x = 1
    let frame = outer(x)
    return x - 10 
