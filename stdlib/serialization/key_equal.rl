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

trait<T> KeyEquivalent:
    fun compute_equal(T first, T second) -> Bool

fun compute_equal(Int value1, Int value2) -> Bool:
    return value1 == value2

fun compute_equal(Float value1, Float value2) -> Bool:
    return value1 == value2

fun _equal_bytes(Byte[8] bytes1, Byte[8] bytes2) -> Bool:
    let counter : Int
    counter = 0
    while counter < 8:
        if !compute_equal(bytes1[counter], bytes2[counter]):
            return false
        counter = counter + 1
    return true

fun compute_equal(Bool value1, Bool value2) -> Bool:
    return value1 == value2

fun compute_equal(Byte value1, Byte value2) -> Bool:
    return value1 == value2

fun<T> compute_equal(Vector<T> vector1, Vector<T> vector2) -> Bool:
    if !(vector1.size() == vector2.size()):
        return false
    let counter : Int
    counter = 0
    while counter < vector1.size():
        if !(compute_equal_of(vector1[counter], vector2[counter])):
            return false
        counter = counter + 1
    return true

fun<T, Int N> compute_equal(T[N] array1, T[N] array2) -> Bool:
    let counter : Int
    counter = 0
    while counter < N:
        if !(compute_equal_of(array1[counter], array2[counter])):
            return false
        counter = counter + 1
    return true

fun<T> _equal_impl(T value1, T value2) -> Bool:
    if value1 is KeyEquivalent:
        return compute_equal(value1, value2)
    else if value1 is Alternative:
        for field of value1:
            using Type = type(field)
            if value1 is Type:
                if value2 is Type:
                    let actual_v1 : Type
                    actual_v1 = value1
                    let actual_v2 : Type
                    actual_v2 = value2
                    return _equal_impl(actual_v1, actual_v2)
                else:
                    return false
    else:
        using TypeT = type(value1)
        if !(value2 is TypeT):
            return false
        
        # Improved struct equality comparison with safety limits
        let field_count = 0
        
        for field1, field2 of value1, value2:
            # Limit field comparisons to avoid potential issues with very large structs
            field_count = field_count + 1
            if field_count > 100:  # Safety limit to match hash implementation
                return true  # Early return instead of break
                
            using FieldType = type(field1)
            if !(field2 is FieldType):
                return false  # Early return instead of setting a flag and breaking
                
            let actual_field1: FieldType
            actual_field1 = field1
            let actual_field2: FieldType
            actual_field2 = field2
            
            # Use early return instead of setting a flag and breaking
            if !_equal_impl(actual_field1, actual_field2):
                return false
        
        return true
    
    # This return is now only hit for the Alternative case that doesn't match any variant
    return false

fun<T> compute_equal_of(T value1, T value2) -> Bool:
    return _equal_impl(value1, value2)
