# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import action
import serialization.print
import bounded_arg

using Alt = Bool | BoundedArg<0, 10>

fun main() -> Int:
    let content : Alt 
    let result = enumerate(content)
    print(result)
    if result.size() != 12:
        return 1
    return 0
