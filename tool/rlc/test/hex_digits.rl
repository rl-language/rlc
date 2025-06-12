# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
    return 0xFF - 255

