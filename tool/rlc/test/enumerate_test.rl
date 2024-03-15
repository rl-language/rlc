# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import action
import serialization.print

ent Content:
    Bool asd
    Bool asd2

fun main() -> Int:
    let content : Content
    let result = enumerate(content)
    if result.size() != 4:
        return 1
    if !result.get(0).asd or !result.get(0).asd2:
        return -1
    if result.get(3).asd or result.get(3).asd2:
        return -1
    if !result.get(1).asd or result.get(1).asd2:
        return -1
    if result.get(2).asd or !result.get(2).asd2:
        return -1
    return 0
