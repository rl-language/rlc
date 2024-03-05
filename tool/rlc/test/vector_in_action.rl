# RUN: rlc %s -o %t -i %stdlib --sanitize -g
# RUN: %t

import collections.vector

act has_member() -> HM:
    frm member: Vector<Int>  
    member.append(10)

fun main() -> Int:
    let hm = has_member()
    return 0
