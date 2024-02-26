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

ent String:
    Vector<Byte> _data 

    fun init():
        self._data.init()
        self._data.append('\0')

    fun append(Byte b):
        self._data.back() = b
        self._data.append('\0')

    fun get(Int index) -> ref Byte:
        return self._data.get(index)

    fun size() -> Int:
        return self._data.size() - 1

    fun append(StringLiteral str):
        self._data.pop()
        let val = 0
        while str[val] != '\0':
            self._data.append(str[val]) 
            val = val + 1
        self._data.append('\0')

    fun append(String str):
        self._data.pop()
        let val = 0
        while val < str.size():
            self._data.append(str.get(val)) 
            val = val + 1
        self._data.append('\0')

    fun add(String other) -> String:
        let to_ret : String
        to_ret.append(self)
        to_ret.append(other)
        return to_ret

    fun equal(StringLiteral other) -> Bool:
        let counter = 0
        while counter < self.size():
            if self.get(counter) != other[counter]:
                return false
            if other[counter] == '\0':
                return false
            counter = counter + 1
        return true

    fun equal(String other) -> Bool:
        if other.size() != self.size():
            return false
        let counter = 0
        while counter < self.size():
            if self.get(counter) != other.get(counter):
                return false
            counter = counter + 1
        return true

    fun drop_back(Int quantity):
        self._data.drop_back(quantity)

    fun back() -> ref Byte:
        return self._data.get(self._data.size() - 2)

    fun reverse():
        let x = 0
        let y = self.size() - 1
        while x < y:
            let tmp = self._data.get(x)
            self._data.get(x) = self._data.get(y)
            self._data.get(y) = tmp
            x = x + 1
            y = y - 1

fun s(StringLiteral literal) -> String:
    let to_return : String
    to_return.append(literal)
    return to_return

ext fun append_to_string(Int x, String output)

ext fun append_to_string(Byte x, String output) 

ext fun append_to_string(Float x, String output)

fun append_to_string(Bool x, String output):
    if x:
        output.append("true")
    else:
        output.append("false")

trait<T> StringSerializable:
    fun append_to_string(T to_add, String output)

fun<T, Int X> append_to_string(T[X] to_add, String output):
    let counter = 0
    output.append('[')

    while counter < X:
        _to_string_impl(to_add[counter], output)
        counter = counter + 1
        if counter != X:
            output.append(", ")

    output.append(']')

fun<T> append_to_string(Vector<T> to_add, String output):
    let counter = 0
    output.append('[')
    while counter < to_add.size():
        _to_string_impl(to_add.get(counter), output)
        counter = counter + 1
        if counter != to_add.size():
            output.append(", ")

    output.append(']')

fun<T> _to_string_impl(T to_add, String output):
    if to_add is StringSerializable:
        to_add.append_to_string(output)
    else if to_add is Alternative:
        let counter = 0
        for field of to_add:
            using Type = type(field)
            if to_add is Type:
                _to_string_impl(counter, output)
                _to_string_impl(to_add, output)
            counter = counter + 1
    else:
        output.append('{')
        for field of to_add:
            _to_string_impl(field, output)
            output.append(", ")
        output.drop_back(1)
        output.back() = '}'

fun<T> to_string(T to_stringyfi) -> String:
    let to_return : String
    _to_string_impl(to_stringyfi, to_return)
    return to_return
