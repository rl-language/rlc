# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun crash() -> Bool:
    let x : Int[10]
    x[100]
    return false

fun main() -> Int:
    if true or crash():
        if !(false and crash()):
            return 0
    return -1
