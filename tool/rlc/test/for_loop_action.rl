# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import collections.vector

act asd() -> Action:
    frm sum = 0
    let v : Vector<Int>
    v.append(0)
    v.append(1)
    v.append(2)
    for i in v: 
        sum = sum + i 
    act inner()

fun main() -> Int:
    let var = asd()
    if var.sum == 3:
        return 0
    return 1

