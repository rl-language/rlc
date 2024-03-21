#
#Copyright 2024 Massimo Fioravanti
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#
import action
import collections.vector
import string

ent<T> Hidden:
    T value

    fun assign(T content):
        self.value = content

fun<T> write_in_observation_tensor(Hidden<T> obj, Vector<Float> output, Int index):
    return

fun<T> size_as_observation_tensor(Hidden<T> obj) -> Int:
    return 0 

fun<T> append_to_vector(Hidden<T> to_add, Vector<Byte> output):
    append_to_byte_vector(to_add.value, output)

fun<T> parse_from_vector(Hidden<T> to_add, Vector<Byte> output, Int index) -> Bool:
    return from_byte_vector(to_add.value, output, index)

fun<T> append_to_string(Hidden<T> to_add, String output):
    to_string(to_add.value, output)

fun<T> parse_string(Hidden<T> to_add, String input, Int index) -> Bool:
    return from_string(to_add.value, input, index) 

