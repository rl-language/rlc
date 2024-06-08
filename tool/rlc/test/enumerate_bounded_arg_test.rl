# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import action
import serialization.print
import bounded_arg

cls Content:
    Bool asd
    BInt<3, 13> asd2


fun main() -> Int:
    let content : Content
    let result = enumerate(content)
    if result.size() != 20:
        return 1
    print(result)
    if result.back().asd2.value != 12:
        return -1
    return 0
