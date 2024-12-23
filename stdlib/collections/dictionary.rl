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

import serialization.to_hash
import serialization.print
import serialization.key_equal
import none

cls<KeyType, ValueType> Entry:
    Bool occupied
    Int hash
    KeyType key
    ValueType value

cls<KeyType, ValueType>Dict:
    OwningPtr<Entry<KeyType, ValueType>> _entries
    Int _size
    Int _capacity
    Float _max_load_factor
    
    fun init():
        self._capacity = 4
        self._size = 0
        self._max_load_factor = 0.75
        self._entries = __builtin_malloc_do_not_use<Entry<KeyType, ValueType>>(self._capacity)
        let counter = 0
        while counter < self._capacity:
            __builtin_construct_do_not_use(self._entries[counter])
            counter = counter + 1
    
    fun insert(KeyType key, ValueType value):
        let load_factor : Float
        load_factor = float(self._size + 1) / float(self._capacity)
        if load_factor > self._max_load_factor:
            self._grow()
        self._insert(self._entries, key, value)
    
    fun _insert(OwningPtr<Entry<KeyType, ValueType>> entries, KeyType key, ValueType value):
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0

        while true:
            let entry = entries[index]
            if !entry.occupied:
                entry.occupied = true
                entry.hash = hash
                entry.key = key
                entry.value = value
                entries[index] = entry  # Update the actual entry in entries
                self._size = self._size + 1
                return
            else if entry.hash == hash and compute_equal_of(entry.key, key):
                entry.value = value
                return
            else:
                let existing_entry_distance = (index + self._capacity - (entry.hash % self._capacity)) % self._capacity
                if existing_entry_distance < distance:
                    let temp_entry = entry
                    entry.hash = hash
                    entry.key = key
                    entry.value = value
                    hash = temp_entry.hash
                    key = temp_entry.key
                    value = temp_entry.value
                    distance = existing_entry_distance
                distance = distance + 1
                index = (index + 1) % self._capacity
    
    fun get(KeyType key) -> Nothing | ValueType:
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let to_return : Nothing | ValueType
        to_return = none()

        while true:
            let entry = self._entries[index]
            
            if !entry.occupied:
                break
            else if entry.hash == hash and compute_equal_of(entry.key, key):
                to_return = entry.value
                break
            else:
                let existing_entry_distance = (index + self._capacity - (entry.hash % self._capacity)) % self._capacity
                if existing_entry_distance < distance:
                    break
                distance = distance + 1
                index = (index + 1) % self._capacity
        return to_return
    
    fun contains(KeyType key) -> Bool:
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let to_return : Bool
        to_return = false

        while true:
            let entry = self._entries[index]
            
            if !entry.occupied:
                break
            else if entry.hash == hash and compute_equal_of(entry.key, key):
                to_return = true
                break
            else:
                let existing_entry_distance = (index + self._capacity - (entry.hash % self._capacity)) % self._capacity
                if existing_entry_distance < distance:
                    break
                distance = distance + 1
                index = (index + 1) % self._capacity
        return to_return
    
    fun remove(KeyType key) -> Bool:
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0

        while true:
            let entry = self._entries[index]
            
            if !entry.occupied:
                break
            else if entry.hash == hash and compute_equal_of(entry.key, key):
                __builtin_destroy_do_not_use(self._entries[index])
                __builtin_construct_do_not_use(self._entries[index])
                return true
            else:
                let existing_entry_distance = (index + self._capacity - (entry.hash % self._capacity)) % self._capacity
                if existing_entry_distance < distance:
                    break
                distance = distance + 1
                index = (index + 1) % self._capacity
        return false
    
    fun keys() -> Vector<KeyType>:
        let to_return : Vector<KeyType>
        let counter = 0
        let index = 0
        while counter < self._size:
            if self._entries[index].occupied:
                to_return.append(self._entries[index].key)
                counter = counter + 1
            index = index + 1
        return to_return

    fun values() -> Vector<ValueType>:
        let to_return : Vector<ValueType>
        let counter = 0
        let index = 0
        while counter < self._size:
            if self._entries[index].occupied:
                to_return.append(self._entries[index].value)
                counter = counter + 1
            index = index + 1
        return to_return

    # returns true if the
    # size of the dictionary is equal
    # to zero
    fun empty() -> Bool:
        return self._size == 0
    
    # returns true if the
    # size of the dictionary is equal
    # to zero
    fun size() -> Int:
        return self._size

    # erases all the elements
    # of the dictionary
    fun clear():        
        while !self.empty():
            self._size = self._size - 1
            __builtin_destroy_do_not_use(self._entries[self._size])
            __builtin_construct_do_not_use(self._entries[self._size])

    fun _grow():
        self._capacity = self._capacity * 2

        let new_entries : OwningPtr<Entry<KeyType, ValueType>>
        new_entries = __builtin_malloc_do_not_use<Entry<KeyType, ValueType>>(self._capacity)
        let counter = 0
        while counter < self._capacity:
            __builtin_construct_do_not_use(new_entries[counter])
            counter = counter + 1

        counter = 0
        let index = 0
        let elements_to_copy = self._size
        self._size = 0
        while counter < elements_to_copy:
            if self._entries[index].occupied:
                self._insert(new_entries, self._entries[index].key, self._entries[index].value)
                counter = counter + 1
            index = index + 1
        
        self._entries = new_entries
        return
