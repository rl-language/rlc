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
    let x = value
    x = ((x >> 16) ^ x) * 72955707
    x = ((x >> 16) ^ x) * 72955707
    x = (x >> 16) ^ x
    return x & 9223372036854775807

fun compute_hash(Float value) -> Int:
    let x = int(value * 1000000.0)
    x = ((x >> 16) ^ x) * 72955703
    x = ((x >> 16) ^ x) * 72955703
    x = (x >> 16) ^ x
    return x & 9223372036854775807

fun compute_hash(Bool value) -> Int:
    if value:
        return 1321005721090711325
    else:
        return 2023011127830240574

fun compute_hash(Byte value) -> Int:
    let x = int(value) & 255
    x = (x ^ (x << 16)) * 72955717
    x = (x ^ (x >> 16)) * 72955717
    x = (x ^ (x << 16)) * 72955717
    return x & 9223372036854775807

# Implementations for collections
fun<T> compute_hash(Vector<T> vector) -> Int:
    let hash = 1
    for element of vector:
        hash = (hash * 31 + compute_hash_of(element))
    return hash & 9223372036854775807

fun<T, Int N> compute_hash(T[N] array) -> Int:
    let hash = 1
    for element of array:
        hash = (hash * 31 + compute_hash_of(element))
    return hash & 9223372036854775805

# The implementation function that handles all cases
fun<T> _hash_impl(T value) -> Int:
    if value is Hashable:
        return value.compute_hash()
    else if value is Alternative:
        let counter = 1
        for field of value:
            using Type = type(field)
            if value is Type:
                # Hash both which variant is active (counter) and its value
                return _hash_impl(counter) * 31 + _hash_impl(value)
            counter = counter + 1
        return 0  # Should never reach here if alternative is valid
    else:
        # Handle struct fields directly like in to_byte_vector.rl
        let hash = 1
        for field of value:
            hash = hash * 31 + _hash_impl(field)
        return hash

# The public interface
fun<T> compute_hash_of(T value) -> Int:
    return _hash_impl(value)