# Copyright 2024 Samuele Pasini
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

trait<T> Hashable:
    fun compute_hash(T to_hash) -> Int

# Specialized implementations for basic types
fun compute_hash(Int value) -> Int:
    return value % 10

fun compute_hash(Float value) -> Int:
    return 1  # TODO: proper float hash

fun compute_hash(Bool value) -> Int:
    return 1

fun compute_hash(Byte value) -> Int:
    return 1

# Implementations for collections
fun<T> compute_hash(Vector<T> vector) -> Int:
    return 1  # TODO: proper vector hash

fun<T, Int N> compute_hash(T[N] array) -> Int:
    return 1  # TODO: proper array hash

# The implementation function that handles all cases
fun<T> _hash_impl(T value) -> Int:
    if value is Hashable:
        return value.compute_hash()
    else if value is Alternative:
        return 99  # TODO: proper alternative hash
    else:
        # Handle struct fields directly like in to_byte_vector.rl
        let hash = 1
        for field of value:
            hash = hash * 31 + _hash_impl(field)
        return hash

# The public interface
fun<T> compute_hash_of(T value) -> Int:
    return _hash_impl(value)