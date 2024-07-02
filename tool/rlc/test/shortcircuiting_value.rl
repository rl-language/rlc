# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun f() {1 >= 0 and 0 < 2, true, (0 == 0 or 1 == 1), true}:
    return

fun g() -> Bool {can f()}:
    return can f()

fun main() -> Int:
    return 1 - int(can g())
