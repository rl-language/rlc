# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import action

cls Asd:
    Bool i

cls Asd2:
    Asd i

cls Asd3:
    Asd2 i

fun main() -> Int:
    let asd : Asd2
    let result = enumerate(asd)
    if result.size() != 2:
        return 1
    return 0
