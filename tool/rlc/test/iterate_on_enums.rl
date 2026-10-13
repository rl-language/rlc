# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
import collections.vector
import serialization.print
import enum_utils

enum X:
    first
    second

fun main() -> Int:
    let v : Vector<X>
    for e in range(X::first):
        v.append(e) 
    
    if v.size() != 2:
        return 1
    if v[0].value != X::first.value:
        return 2
    if v[1].value != X::second.value:
        return 3
    return 0
