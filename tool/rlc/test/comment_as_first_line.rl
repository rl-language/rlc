# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act asd() -> A: #asd
    # asd
    return

fun main() -> Int: #asd
    # asd
    return 0 #asd
