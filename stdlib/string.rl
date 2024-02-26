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

ext fun to_string(Int x) -> String

ext fun to_string(Byte x) -> String

ext fun to_string(Float x) -> String

fun to_string(Bool x) -> String:
    if x:
        return "true"s
    return "false"s
