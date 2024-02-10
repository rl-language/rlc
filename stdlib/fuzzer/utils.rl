# Copyright 2024 Cem Cebeci
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
import fuzzer.cpp_functions

fun fuzzer_init_available_subactions() -> Vector<Int>:
    let res : Vector<Int>
    return res

fun fuzzer_add_available_subaction(Vector<Int> available_subactions, Int subactionID):
    available_subactions.append(subactionID)

fun fuzzer_pick_subaction(Vector<Int> available_subactions) -> Int:
    let index = fuzzer_get_input( available_subactions.size())
    return available_subactions.get(index)

fun fuzzer_clear_available_subactions(Vector<Int> available_subactions):
    available_subactions.clear()
