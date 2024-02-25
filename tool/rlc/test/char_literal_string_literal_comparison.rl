# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
    if "1"[0] == '1':
        return 0
    return 1
