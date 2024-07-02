# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

trait<T, T2> Comparable:
    fun equal(T a, T2 b) -> Bool

fun equal(Float b, Int a) -> Bool:
    return true

fun main() -> Int:
    if 3 is Comparable<Float>:
        return 0
    return 1
