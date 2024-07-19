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

# A integer with a max and min, so that
# enumerate will return the range of values
# between the two.
cls<Int min, Int max> BInt:
    Int value

    fun equal(Int other) -> Bool:
        return self.value == other

    fun equal(BInt<min, max> other) -> Bool:
        return self.value == other.value

    fun less(BInt<min, max> other) -> Bool:
        return self.value < other.value

    fun less(Int other) -> Bool:
        return self.value < other

    fun greater(BInt<min, max> other) -> Bool:
        return self.value > other.value

    fun greater(Int other) -> Bool:
        return self.value > other

    fun greater_equal(BInt<min, max> other) -> Bool:
        return self.value >= other.value

    fun greater_equal(Int other) -> Bool:
        return self.value >= other

    fun less_equal(BInt<min, max> other) -> Bool:
        return self.value <= other.value

    fun less_equal(Int other) -> Bool:
        return self.value <= other

    fun assign(Int other):
        self.value = other
        if self.value >= max:
            self.value = max - 1 
        if self.value < min:
            self.value = min 

    fun not_equal(Int other) -> Bool:
        return self.value != other

    fun not_equal(BInt<min, max> other) -> Bool:
        return self.value != other.value

    fun add(Int val) -> BInt<min, max>:
        let other : BInt<min, max>
        other.value = val
        return self + other

    fun add(BInt<min, max> other) -> BInt<min, max>:
        let to_return : BInt<min, max>
        to_return.value = self.value + other.value
        if to_return.value >= max:
            to_return.value = max - 1
        if to_return.value < min:
            to_return.value = min
        return to_return

    fun mul(BInt<min, max> other) -> BInt<min, max>:
        let to_return : BInt<min, max>
        to_return.value = self.value * other.value
        if to_return.value >= max:
            to_return.value = max - 1
        if to_return.value < min:
            to_return.value = min
        return to_return

    fun reminder(Int val) -> BInt<min, max>:
        let to_return : BInt<min, max>
        let value = self.value % val
        to_return = value
        return to_return 

    fun reminder(BInt<min, max> val) -> BInt<min, max>:
        let to_return : BInt<min, max>
        let value = self.value % val.value
        to_return = value
        return to_return 

    fun mul(Int val) -> BInt<min, max>:
        let other : BInt<min, max>
        other.value = val
        return self * other

    fun sub(BInt<min, max> other) -> BInt<min, max>:
        let to_return : BInt<min, max>
        to_return.value = self.value - other.value
        if to_return.value >= max:
            to_return.value = max - 1
        if to_return.value < min:
            to_return.value = min
        return to_return

    fun sub(Int val) -> BInt<min, max>:
        let other : BInt<min, max>
        other.value = val
        return self - other


fun<Int min, Int max> max(BInt<min, max> l, BInt<min, max> r) -> BInt<min, max>:
  if l < r:
    return r
  return l


fun<Int min, Int max> min(BInt<min, max> l, BInt<min, max> r) -> BInt<min, max>:
  if r < l:
    return r
  return l

fun<Int min, Int max> append_to_vector(BInt<min, max> to_add, Vector<Byte> output):
    if max - min < 256:
        let to_append = byte(to_add.value - min - 128)
        append_to_vector(to_append, output)
    else:
        let to_append = to_add.value - min
        append_to_vector(to_append, output)

fun<Int min, Int max> parse_from_vector(BInt<min, max> to_add, Vector<Byte> output, Int index) -> Bool:
    if max - min < 256:
        let value : Byte 
        if !parse_from_vector(value, output, index):
            return false
        let value_casted = int(value) + 128
        value_casted = value_casted % (max - min)
        to_add.value = int(value_casted) + min
        return true
    else:
        let value : Int
        if !parse_from_vector(value, output, index):
            return false
        to_add.value = (value % (max - min)) + min
        return true

fun<Int min, Int max> append_to_string(BInt<min, max> to_add, String output):
    append_to_string(to_add.value, output)

fun<Int min, Int max> parse_string(BInt<min, max> to_add, String input, Int index) -> Bool:
    if !parse_string(to_add.value, input, index):
        return false
    if to_add.value < min or to_add.value >= max:
        return false
    return true

fun<Int min, Int max> enumerate(BInt<min, max> to_add, Vector<BInt<min, max>> output):
    let counter = min
    while counter < max:
        let x : BInt<min, max>
        x.value = counter
        output.append(x)
        counter = counter + 1

fun<Int min, Int max> tensorable_warning(BInt<min, max> x, String out):
    return
