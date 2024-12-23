# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
cls Asd:
    Int y

act inner(ctx Int inner) -> Inner:
    act something()

act outer() -> Outer:
    frm asd : Asd
    subaction*(2) i = inner(asd.y)

fun main() -> Int:
    return 0
