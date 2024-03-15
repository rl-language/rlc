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
import serialization.to_byte_vector
import string 

ent<Int min, Int max> BoundedArg:
    Int value

    fun equal(Int other) -> Bool:
        return self.value == other

    fun equal(BoundedArg<min, max> other) -> Bool:
        return self.value == other.value

    fun not_equal(BoundedArg<min, max> other) -> Bool:
        return self.value != other.value

    fun add(BoundedArg<min, max> other) -> BoundedArg<min, max>:
        let to_return : BoundedArg<min, max>
        to_return.value = self.value + other.value
        if to_return.value >= max:
            to_return.value = max - 1
        if to_return.value < min:
            to_return.value = min
        return to_return

    fun sub(BoundedArg<min, max> other) -> BoundedArg<min, max>:
        let to_return : BoundedArg<min, max>
        to_return.value = self.value - other.value
        if to_return.value >= max:
            to_return.value = max - 1
        if to_return.value < min:
            to_return.value = min
        return to_return

fun<Int min, Int max> append_to_vector(BoundedArg<min, max> to_add, Vector<Byte> output):
    if max - min < 256:
        let to_append = byte(to_add.value - min)
        append_to_vector(to_append, output)
    else:
        let to_append = to_add.value - min
        append_to_vector(to_append, output)

fun<Int min, Int max> parse_from_vector(BoundedArg<min, max> to_add, Vector<Byte> output, Int index) -> Bool:
    if max - min < 256:
        let value : Byte 
        if !parse_from_vector(value, output, index):
            return false
        value = value % byte(max - min)
        to_add.value = int(value) + min
        return true
    else:
        let value : Int
        if !parse_from_vector(value, output, index):
            return false
        to_add.value = (value % (max - min)) + min
        return true

fun<Int min, Int max> append_to_string(BoundedArg<min, max> to_add, String output):
    append_to_string(to_add.value, output)

fun<Int min, Int max> parse_string(BoundedArg<min, max> to_add, String input, Int index) -> Bool:
    if !parse_string(to_add.value, input, index):
        return false
    if to_add.value < min or to_add.value >= max:
        return false
    return true

fun<Int min, Int max> enumerate(BoundedArg<min, max> to_add, Vector<BoundedArg<min, max>> output):
    let counter = min
    while counter < max:
        let x : BoundedArg<min, max>
        x.value = counter
        output.append(x)
        counter = counter + 1
