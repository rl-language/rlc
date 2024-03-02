# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent Asd:
    Int content

using T = Asd 

ent Tasd:
    T content

fun main() -> Int:
    let x : Tasd
    if x.content is Asd:
        return 0
    return 1

