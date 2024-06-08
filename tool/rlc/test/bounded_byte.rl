# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import serialization.to_byte_vector
import collections.vector

cls<Int Min, Int Max> BoundedByte:
    Byte _value

    fun get() -> Byte:
        return self._value

fun<Int Min, Int Max> parse_from_vector(BoundedByte<Min, Max> to_add, Vector<Byte> input, Int index) -> Bool:
    if input.size() < index:
        return false
    let value = input.get(index)
    if value < byte(Min):
        value = byte(Min)
    if value >= byte(Max):
        value = byte(Max - 1)
    to_add._value = value
    index = index + 1
    return true

fun main() -> Int:
    let val : BoundedByte<0, 4>
    let vector : Vector<Byte>
    vector.append(byte(2))
    from_byte_vector(val, vector)
    if val.get() != byte(2):
        return -1
    vector.pop()
    vector.append(byte(10))
    from_byte_vector(val, vector)
    if val.get() != byte(3):
        return -1
    return 0 

