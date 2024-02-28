# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import string

fun main() -> Int:
    let parsed : Bool 
    if parse_string(parsed, " true false "s, 0) and parsed:
        return 0
    else:
        return 1

