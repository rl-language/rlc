## Copyright 2025 Massimo Fioravanti
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##    http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

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
