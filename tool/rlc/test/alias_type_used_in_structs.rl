# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls Asd:
    Int content

using T = Asd 

cls Tasd:
    T content

fun main() -> Int:
    let x : Tasd
    if x.content is Asd:
        return 0
    return 1

