# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
    if '1' + byte(1) == '2':
        return 0
    return 1
