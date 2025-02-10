## Copyright 2024 Samuele Pasini
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

import collections.dictionary

# Define LargeKey type to match C++ LargeKey
cls LargeKey:
    Int content
    Int[10] dont_care

    fun init():
        self.content = 0
        let i = 0
        while i < 10:
            self.dont_care[i] = 0
            i = i + 1

# Explicit instantiations for benchmarking
cls DictIntInt:
    Dict<Int, Int> dict

cls DictLargeKeyInt:
    Dict<LargeKey, Int> dict

fun init__DictIntInt(DictIntInt self):
    self.dict.init()

fun init__DictLargeKeyInt(DictLargeKeyInt self):
    self.dict.init() 