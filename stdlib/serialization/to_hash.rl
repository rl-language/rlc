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
    # Improved integer hash function based on MurmurHash3
    let x = value
    x = x ^ (x >> 33)
    x = x * 1099511628211  # Smaller prime, still effective for hashing
    x = x ^ (x >> 33)
    x = x * 16777619  # Another effective hash multiplier
    x = x ^ (x >> 33)
    return x & 9223372036854775807  # Ensure positive value (mask with INT64_MAX)

fun compute_hash(Float value) -> Int:
    # Improved float hash using bit representation
    # First convert to bits, then hash those bits as an integer
    # For simplicity, we're still using the multiplication approach
    # A better implementation would use IEEE 754 bit representation
    let x = int(value * 1000000.0)
    x = x ^ (x >> 33)
    x = x * 1099511628211
    x = x ^ (x >> 33)
    x = x * 16777619
    x = x ^ (x >> 33)
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

# Add String hashing - assuming a String type exists
# Using FNV-1a hash algorithm which is fast and has good distribution
fun compute_hash(String str) -> Int:
    let hash = 2166136261  # Smaller FNV offset basis to avoid issues with large constants
    let fnv_prime = 16777619  # Smaller FNV prime
    
    let i = 0
    while i < str.size():
        let char_value = int(str[i])
        hash = hash ^ char_value
        hash = (hash * fnv_prime) & 9223372036854775807
        i = i + 1
    
    return hash

# Implementations for collections
fun<T> compute_hash(Vector<T> vector) -> Int:
    let hash = 1
    for element of vector:
        # Improved combination formula
        hash = (hash * 31 + compute_hash_of(element)) & 9223372036854775807
    return hash

fun<T, Int N> compute_hash(T[N] array) -> Int:
    let hash = 1
    for element of array:
        # Improved combination formula
        hash = (hash * 31 + compute_hash_of(element)) & 9223372036854775807
    return hash

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
                return (_hash_impl(counter) * 31 + _hash_impl(value)) & 9223372036854775807
            counter = counter + 1
        return 0  # Should never reach here if alternative is valid
    else:
        # Improved struct hashing that's more resilient across platforms
        let hash = 17  # Start with a prime number
        let field_count = 0
        
        for field of value:
            # Use a prime number multiplier for better distribution
            # and limit to a reasonable number of fields to avoid excessive computation
            field_count = field_count + 1
            if field_count > 100:  # Safeguard against structs with too many fields
                return hash  # Early return instead of break
                
            # Mix the field hash more carefully to avoid bits canceling each other
            let field_hash = _hash_impl(field)
            # Rotate the hash a bit to spread influence of each field
            hash = ((hash << 5) + hash) ^ field_hash
            # Ensure we stay in positive range
            hash = hash & 9223372036854775807
        
        return hash

# The public interface
fun<T> compute_hash_of(T value) -> Int:
    return _hash_impl(value)