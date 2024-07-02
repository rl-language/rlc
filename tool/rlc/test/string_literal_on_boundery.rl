# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
# RUN: rlc %s -o %t --header 
# RUN: rlc %s -o %t --python

fun f() -> StringLiteral:
    return "asd"

fun main() -> Int:
    return int(f()[0] == "asda"[3]) - 1
