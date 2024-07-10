# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import machine_learning
import action

enum Asd:
    first
    second

    fun equal(Asd other) -> Bool:
        return self.value == other.value

    fun not_equal(Asd other) -> Bool:
        return self.value != other.value

fun main() -> Int:
    let enumeration = enumerate(Asd::first)
    if enumeration.size() != 2:
        return -3
    if enumeration.get(0) != Asd::first:
        return -2
    if enumeration.get(1) != Asd::second:
        return -1
    return 0
