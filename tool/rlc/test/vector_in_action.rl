# RUN: rlc %s -o %t -i %stdlib --sanitize -g
# RUN: %t

import collections.vector

act has_member() -> HM:
    frm member: Vector<Int>  

fun main() -> Int:
    let hm = has_member()
    return 0
