# Copyright 2024 Massimo Fioravanti
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import collections.vector

# Trait that must be implemented by a type 
# to override the standad way it is added to
# a vector of bytes
trait<T> ByteVectorSerializable:
    fun append_to_vector(T to_add, Vector<Byte> output)

fun append_to_vector(Int to_add, Vector<Byte> output):
    let array = __builtin_to_array(to_add)  
    let counter = 0
    while counter < 8:
        output.append(array[counter])
        counter = counter + 1 

fun append_to_vector(Float to_add, Vector<Byte> output):
    let array = __builtin_to_array(to_add)  
    let counter = 0
    while counter < 8:
        output.append(array[counter])
        counter = counter + 1 

fun append_to_vector(Bool to_add, Vector<Byte> output):
    let array = __builtin_to_array(to_add)  
    output.append(array[0])

fun append_to_vector(Byte to_add, Vector<Byte> output):
    output.append(to_add)

fun<T> append_to_vector(Vector<T> to_add, Vector<Byte> output):
    append_to_vector(to_add.size(), output)
    let counter = 0
    while counter < to_add.size():
        _to_vector_impl(to_add.get(counter), output)
        counter = counter + 1 

fun<T, Int X> append_to_vector(T[X] to_add, Vector<Byte> output):
    let counter = 0
    while counter < X:
        _to_vector_impl(to_add[counter], output)
        counter = counter + 1

fun<T> _to_vector_impl(T to_add, Vector<Byte> output):
    if to_add is ByteVectorSerializable:
        to_add.append_to_vector(output)
    else if to_add is Alternative:
        let counter = 0
        for field of to_add:
            using Type = type(field)
            if to_add is Type:
                _to_vector_impl(counter, output)
                _to_vector_impl(to_add, output)
            counter = counter + 1
    else:
        for field of to_add:
            _to_vector_impl(field, output)

# converts `to_convert` to a sequence of bytes and adds it
# to `out`.
fun<T> append_to_byte_vector(T to_convert, Vector<Byte> out):
    _to_vector_impl(to_convert, out)

# converts `to_convert` to a sequence of bytes 
fun<T> as_byte_vector(T to_convert) -> Vector<Byte>:
    let vec : Vector<Byte>
    _to_vector_impl(to_convert, vec)
    return vec

# Trait that can be implemented to override the default conversion
# from a array of bytes to a object.
# It must be implemented if the trait has implemented ByteVectorSerializable
trait<T> ByteVectorParsable:
    fun parse_from_vector(T result, Vector<Byte> input, Int index) -> Bool

fun parse_from_vector(Int result, Vector<Byte> input, Int index) -> Bool:
    if index + 8 > input.size():
        return false
    let to_parse : Byte[8]
    let counter = 0
    while counter < 8:
        to_parse[counter] = input.get(index)
        index = index + 1   
        counter = counter + 1 
    result = __builtin_from_array<Int>(to_parse)    
    return true

fun parse_from_vector(Float result, Vector<Byte> input, Int index) -> Bool:
    if input.size() < index + 8:
        return false
    let to_parse : Byte[8]
    let counter = 0
    while counter < 8:
        to_parse[counter] = input.get(index)
        index = index + 1   
        counter = counter + 1 
    result = __builtin_from_array<Float>(to_parse)  
    return true

fun parse_from_vector(Bool result, Vector<Byte> input, Int index) -> Bool:
    if input.size() <= index:
        return false
    let to_parse : Byte[1]
    to_parse[0] = input.get(index)
    index = index + 1   
    result = __builtin_from_array<Bool>(to_parse)   
    return true

fun parse_from_vector(Byte result, Vector<Byte> input, Int index) -> Bool:
    if input.size() <= index:
        return false
    result = input.get(index)
    index = index + 1
    return true

fun<T> parse_from_vector(Vector<T> output, Vector<Byte> input, Int index) -> Bool:
    let size : Int
    if !parse_from_vector(size, input, index):
        return false
    let counter = 0
    while counter < size:
        let raw : T
        if !_from_vector_impl(raw, input, index):
            return false
        output.append(raw)
        counter = counter + 1 
    return true

fun<T, Int X> parse_from_vector(T[X] to_add, Vector<Byte> input, Int index) -> Bool:
    let counter = 0
    while counter < X:
        if !_from_vector_impl(to_add[counter], input, index):
            return false
        counter = counter + 1
    return true

fun<T> _from_vector_impl(T to_add, Vector<Byte> input, Int index) -> Bool:
    if to_add is ByteVectorParsable:
        return to_add.parse_from_vector(input, index)
    else if to_add is Alternative:
        let counter = 0
        if !_from_vector_impl(counter, input, index):
            return false
        for field of to_add:
            if counter == 0:
                using Type = type(field)
                let to_parse : Type
                if !_from_vector_impl(to_parse, input, index):
                    return false
                to_add = to_parse
                return true
            counter = counter - 1
        return false
    else:
        for field of to_add:
            if !_from_vector_impl(field, input, index):
                return false
        return true

# converts the bytes in `input` into a T and 
# assigns the value to `result`. Returns false if the conversion failed.
fun<T> from_byte_vector(T result, Vector<Byte> input) -> Bool:
    return _from_vector_impl(result, input, 0)

# converts the bytes in `input` starting at `read_bytes` into a T and 
# assigns the value to `result`. Returns false if the conversion failed.
# read_bytes is advanced up to the index of the first bytes not used to
# parse `result`
fun<T> from_byte_vector(T result, Vector<Byte> input, Int read_bytes) -> Bool:
    return _from_vector_impl(result, input, read_bytes)
