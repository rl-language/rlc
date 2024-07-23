# RUN: rlc %s -o %t%exeext -i %stdlib --print-ir-on-failure=false 
# RUN %t%exeext | FileCheck %s


# CHECK: failed assert 

fun main() -> Int:
    assert(false, "failed assert")
    return 0
