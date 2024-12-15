# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act inner() -> Inner:
    act asd()

cls Cont:
    Inner content

act outer() -> Outer:
    frm asd : Cont
    subaction* asd.content

fun main() -> Int:
    return 0
