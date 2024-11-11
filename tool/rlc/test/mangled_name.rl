# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import string

fun main() -> Int:
    if s(__builtin_mangled_name(3)) == s("int64_t"):
        return 0
    return 1
