# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls Range:
    Int _size 

    fun get(Int i) -> Int:
        return i

    fun size() -> Int:
        return self._size

    fun set_size(Int new_size):
        self._size = new_size

fun range(Int size) -> Range:
    let range : Range
    range.set_size(size)
    return range

fun main() -> Int:
    let sum = 0
    for i in range(10):
        sum = sum + i 
    if sum == 45:
        return 0
    return 1
