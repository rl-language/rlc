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

# a c style string, composed of a series of ascii characters
# terminated by a null terminator. If the string changes, it 
# may be reallocated, causing all references to its 
# characters to be invalidated.
cls String:
    Vector<Byte> _data 

    fun init():
        self._data.init()
        self._data.append('\0')

    # append the byte b, interpreted as a ascii
    # character to the string
    fun append(Byte b):
        self._data.back() = b
        self._data.append('\0')

    # returns a reference to the character
    # at the `index` location
    fun get(Int index) -> ref Byte:
        return self._data.get(index)

    # returns true if the current string contains the
    # string `lit` starting from the character at `pos`
    fun substring_matches(StringLiteral lit, Int pos) -> Bool:
        if pos >= self.size():
            return false

        let current = 0
        while lit[current] != '\0':
            if lit[current] != self.get(pos + current):
                return false
            current = current + 1
        return true

    # returns the size of the string, excluding
    # the null terminator
    fun size() -> Int:
        return self._data.size() - 1

    # returns the number of occurences
    # a certain character appears in the string
    fun count(Byte b) -> Int:
        let to_return = 0
        let index = 0
        while index != self.size():
            if self.get(index) == b:
                to_return = to_return + 1
            index = index + 1
        return to_return

    # appens `str` to the current string
    fun append(StringLiteral str):
        self._data.pop()
        let val = 0
        while str[val] != '\0':
            self._data.append(str[val]) 
            val = val + 1
        self._data.append('\0')

    # appens `str` to the current string
    fun append(String str):
        self._data.pop()
        let val = 0
        while val < str.size():
            self._data.append(str.get(val)) 
            val = val + 1
        self._data.append('\0')

    # returns the concatenation of this
    # string and `other`, without modifying
    # this string.
    fun add(String other) -> String:
        let to_ret : String
        to_ret.append(self)
        to_ret.append(other)
        return to_ret

    # returns true if every character of this
    # string is equal to every character of
    # the `other` string
    fun equal(StringLiteral other) -> Bool:
        let counter = 0
        while counter < self.size():
            if self.get(counter) != other[counter]:
                return false
            if other[counter] == '\0':
                return false
            counter = counter + 1
        if other[counter] != '\0':
            return false
        return true

    # returns true if every character of this
    # string is equal to every character of
    # the `other` string
    fun equal(String other) -> Bool:
        if other.size() != self.size():
            return false
        let counter = 0
        while counter < self.size():
            if self.get(counter) != other.get(counter):
                return false
            counter = counter + 1
        return true

    fun not_equal(String other) -> Bool:
        return !(self.equal(other))

    fun not_equal(StringLiteral other) -> Bool:
        return !(self.equal(other))

    # removes `quantity` characters from the
    # end of the string
    fun drop_back(Int quantity):
        self._data.drop_back(quantity)

    # returns a reference the last character before 
    # the null terminator of the string
    fun back() -> ref Byte:
        return self._data.get(self._data.size() - 2)

    # reverses inplace the string
    fun reverse():
        let x = 0
        let y = self.size() - 1
        while x < y:
            let tmp = self._data.get(x)
            self._data.get(x) = self._data.get(y)
            self._data.get(y) = tmp
            x = x + 1
            y = y - 1

    # returns a string such that every parantesys
    # in this string generates a new line indented
    # as many times as the number of 
    # parentesys not yet closed
    fun to_indented_lines() -> String:
        let to_return : String

        let counter = 0
        let scopes = 0
        while counter < self.size():
            if is_open_paren(self.get(counter)):
                to_return.append(self.get(counter))
                to_return.append('\n')
                scopes = scopes + 1
                _indent_string(to_return, scopes)
            else if is_close_paren(self.get(counter)):
                to_return.append('\n')
                scopes = scopes - 1
                _indent_string(to_return, scopes)
                to_return.append(self.get(counter))
            else if self.get(counter) == ',':
                to_return.append(self.get(counter))
                to_return.append('\n')
                _indent_string(to_return, scopes)
                if self.get(counter + 1) == ' ':
                    counter = counter + 1
            else:
                to_return.append(self.get(counter))
            counter = counter + 1

        return to_return

fun _indent_string(String output, Int count):
    let counter2 = 0
    while counter2 != count:
        output.append("  ")
        counter2 = counter2 + 1

# transforms a string literal into a String
# ex: let str = "hey"s.add(" bye")
fun s(StringLiteral literal) -> String:
    let to_return : String
    to_return.append(literal)
    return to_return

# Trait that can be implemented by a type to override
# the regular conversion to string for that type.
trait<T> StringSerializable:
    fun append_to_string(T to_add, String output)

# Trait that can be implemented by a type to override
# which name is displayed in serialization of alternative 
# types
trait<T> CustomGetTypeName:
    fun get_type_name(T to_add) -> StringLiteral

fun append_to_string(StringLiteral x, String output):
    output.append(x)

# loads the file at path `file_name`, placing its contents
# inside `out`. returns false if the file could not be read
ext fun load_file(String file_name, String out) -> Bool 
ext fun append_to_string(Int x, String output)
ext fun append_to_string(Byte x, String output) 
ext fun append_to_string(Float x, String output)

fun append_to_string(Bool x, String output):
    if x:
        output.append("true")
    else:
        output.append("false")

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

fun<T, Int dim> append_to_string(BoundedVector<T, dim> to_add, String output):
    let counter = 0
    output.append('[')
    while counter < to_add.size():
        _to_string_impl(to_add.get(counter), output)
        counter = counter + 1
        if counter != to_add.size():
            output.append(", ")

    output.append(']')


fun<T> _print_type(T to_add, StringLiteral default_type_name, String output):
    if to_add is CustomGetTypeName:
        output.append(to_add.get_type_name())
    else:
        _to_string_impl(default_type_name, output)

fun<T> _to_string_impl(T to_add, String output):
    if to_add is StringSerializable:
        to_add.append_to_string(output)
    else if to_add is Alternative:
        let counter = 0
        for name, field of to_add:
            using Type = type(field)
            if to_add is Type:
                _print_type(to_add, name, output)
                output.append(' ')
                _to_string_impl(to_add, output)
            counter = counter + 1
    else:
        output.append('{')
        let added_one = false
        for name, field of to_add:
            _to_string_impl(name, output)
            output.append(": ")
            _to_string_impl(field, output)
            output.append(", ")
            added_one = true
        if added_one:
            output.drop_back(1)
            output.back() = '}'
        else:
            output.append('}')

fun<T> to_string(T to_stringyfi, String output):
    _to_string_impl(to_stringyfi, output)

# convert a object type into a string 
fun<T> to_string(T to_stringyfi) -> String:
    let to_return : String
    _to_string_impl(to_stringyfi, to_return)
    return to_return

# Trait that must be implemented to allow to convert
# string into a type when StringSerializable has been
# overwritten as well.
trait<T> StringParsable:
    fun parse_string(T result, String buffer, Int index) -> Bool

fun is_space(Byte b) -> Bool:
    return b == ' ' or b == '\n' or b == '\t'

ext fun is_alphanumeric(Byte b) -> Bool

fun is_open_paren(Byte b) -> Bool:
    return b == '(' or b == '[' or b == '{'

fun is_close_paren(Byte b) -> Bool:
    return b == ')' or b == '}' or b == ']'

ext fun parse_string(Int result, String buffer, Int index) -> Bool
ext fun parse_string(Byte result, String buffer, Int index) -> Bool
ext fun parse_string(Float result, String buffer, Int index) -> Bool

fun _consume_space(String buffer, Int index):
    while is_space(buffer.get(index)):
        index = index + 1 

# returns the length of a string literal
fun length(StringLiteral literal) -> Int:
    let size = 0
    while literal[size] != '\0':
        size = size + 1
    return size 

fun _consume_literal(String buffer, StringLiteral literal, Int index) -> Bool:
    _consume_space(buffer, index)
    if !buffer.substring_matches(literal, index):
        return false
    let size = length(literal)
    index = index + size
    return true

fun _consume_literal_token(String buffer, StringLiteral literal, Int index) -> Bool:
    _consume_space(buffer, index)
    if !buffer.substring_matches(literal, index):
        return false
    let counter = length(literal)

    # if there is a trailing alphanumeric character it means
    # we did not perfectly matched the name we were looking for
    let next_char = buffer.get(index + counter)
    if is_alphanumeric(next_char) or next_char == '_' or next_char == '-':
        return false

    index = index + counter

    return true

fun parse_string(Bool result, String buffer, Int index) -> Bool:
    _consume_space(buffer, index)
    if buffer.size() == index: 
        return false

    if buffer.substring_matches("true", index):
        result = true
        index = index + 4
    else if buffer.substring_matches("false", index):
        result = false
        index = index + 5
    else:
        return false

    return true

fun<T, Int X> parse_string(T[X] result, String buffer, Int index) -> Bool:
    _consume_space(buffer, index)
    let counter = 0
    if buffer.get(index) != '[':
        return false
    index = index + 1
    _consume_space(buffer, index)

    while counter < X:
        if !_parse_string_impl(result[counter], buffer, index):
            return false
        _consume_space(buffer, index)
        if counter != X:
            if buffer.get(index) != ',':
                return false
            index = index + 1
            _consume_space(buffer, index)

    if buffer.get(index) != ']':
        return false
    index = index + 1
    return true

fun<T> parse_string(Vector<T> result, String buffer, Int index) -> Bool:
    _consume_space(buffer, index)
    let counter = 0
    if buffer.get(index) != '[':
        return false
    index = index + 1
    _consume_space(buffer, index)

    let keep_parsing = true
    while keep_parsing: 
        let to_parse : T
        if !_parse_string_impl(to_parse, buffer, index):
            return false
        result.append(to_parse)
        _consume_space(buffer, index)
        keep_parsing = buffer.get(index) == ','
        if keep_parsing:
            index = index + 1
            _consume_space(buffer, index)

    if buffer.get(index) != ']':
        return false
    index = index + 1
    return true


fun<T, Int x> parse_string(BoundedVector<T, x> result, String buffer, Int index) -> Bool:
    _consume_space(buffer, index)
    let counter = 0
    if buffer.get(index) != '[':
        return false
    index = index + 1
    _consume_space(buffer, index)

    let keep_parsing = true
    while keep_parsing: 
        let to_parse : T
        if !_parse_string_impl(to_parse, buffer, index):
            return false
        result.append(to_parse)
        _consume_space(buffer, index)
        keep_parsing = buffer.get(index) == ','
        if keep_parsing:
            index = index + 1
            _consume_space(buffer, index)

    if buffer.get(index) != ']':
        return false
    index = index + 1
    return true

fun<T> _parse_type(T to_parse, String buffer, StringLiteral type_name, Int index) -> Bool:
    if to_parse is CustomGetTypeName:
        return _consume_literal_token(buffer, to_parse.get_type_name(), index)
    else:
        return _consume_literal_token(buffer, type_name, index)

fun<T> _parse_string_impl(T result, String buffer, Int index) -> Bool:
    if result is StringParsable:
        return result.parse_string(buffer, index)
    else if result is Alternative:
        for name, field of result:
            using Type = type(field)
            if _parse_type(field, buffer, name, index):
                let to_parse : Type 
                if !_parse_string_impl(to_parse, buffer, index):
                    return false
                result = to_parse
                return true
        return false
    else:
        if !_consume_literal(buffer, "{", index):
            return false
        for name, field of result:
            if !_consume_literal_token(buffer, name, index):
                return false
            if !_consume_literal(buffer, ":", index):
                return false
            if !_parse_string_impl(field, buffer, index):
                return false
            _consume_literal(buffer, ",", index)
        if !_consume_literal(buffer, "}", index):
            return false

        return true

fun<T> from_string(T result, String buffer) -> Bool:
    let index = 0
    return _parse_string_impl(result, buffer, index)

# given a String *buffer* and a offset into that string index,
# parses buffer starting from *index* and uses those information
# to set the value of *result*.
fun<T> from_string(T result, String buffer, Int index) -> Bool:
    return _parse_string_impl(result, buffer, index)
