# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

fun main() -> Int:
    let a = 0
    while a != 200000000:
        let x : Int[1000]
        x[1] = 1
        a = a + x[1]
    return 0
