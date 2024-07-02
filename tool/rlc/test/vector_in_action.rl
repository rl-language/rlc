# RUN: rlc %s -o %t -i %stdlib --sanitize -g
# RUN: %t%exeext

import collections.vector

act has_member() -> HM:
    frm member: Vector<Int>  
    member.append(10)
    act asd()

fun main() -> Int:
    let hm = has_member()
    return 0
