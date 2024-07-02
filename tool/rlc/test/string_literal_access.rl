# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
    return int("asd"[0] == "asda"[3]) - 1
