# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act asd() -> A: #asd
    # asd
    act f()
    return

fun main() -> Int: #asd
    # asd
    return 0 #asd
