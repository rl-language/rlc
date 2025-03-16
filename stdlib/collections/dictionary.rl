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
import collections.vector

cls<KeyType, ValueType> Entry:
    Bool occupied
    Int hash
    KeyType key
    ValueType value

cls<KeyType, ValueType> Dict:
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
            self._entries[counter].occupied = false
            counter = counter + 1
    
    fun insert(KeyType key, ValueType value) -> Bool:
        let load_factor : Float
        load_factor = float(self._size + 1) / float(self._capacity)
        if load_factor > self._max_load_factor:
            self._grow()
        self._insert(self._entries, key, value)
        return true
    
    fun _insert(OwningPtr<Entry<KeyType, ValueType>> entries, KeyType key, ValueType value):
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let probe_count = 0  # Add safety counter
        
        # Create local copies of key and value to avoid modifying the input parameters
        let current_key = key
        let current_value = value
        let current_hash = hash

        while true:
            # Add safety check to prevent infinite loops
            if probe_count >= self._capacity:
                assert(false, "Maximum probe count exceeded - likely an implementation bug")
                return
            probe_count = probe_count + 1
            
            
            if !entries[index].occupied:
                __builtin_construct_do_not_use(entries[index])
                let entry = entries[index]
                entry.occupied = true
                entry.hash = current_hash
                entry.key = current_key
                entry.value = current_value
                entries[index] = entry  # Update the actual entry in entries
                self._size = self._size + 1
                return
            else if entries[index].hash == current_hash and compute_equal_of(entries[index].key, current_key):
                ref entry = entries[index]
                entry.value = current_value
                entries[index] = entry  # Update the actual entry in entries
                return
            else:
                ref entry = entries[index]
                let existing_entry_distance = (index + self._capacity - (entry.hash % self._capacity)) % self._capacity
                if existing_entry_distance < distance:
                    let temp_entry = entry
                    entry.hash = current_hash
                    entry.key = current_key
                    entry.value = current_value
                    entries[index] = entry  # Update the swapped entry
                    current_hash = temp_entry.hash
                    current_key = temp_entry.key
                    current_value = temp_entry.value
                    distance = existing_entry_distance
                distance = distance + 1
                index = (index + 1) % self._capacity
    
    fun get(KeyType key) -> ValueType:
        # Quick return for empty dictionary
        if self._size == 0:
            assert(false, "key not found in empty dictionary")
        
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let probe_count = 0  # Add safety counter

        while true:
            # Add safety check to prevent infinite loops
            if probe_count >= self._capacity:
                assert(false, "GET: Maximum probe count exceeded - likely an implementation bug")
            probe_count = probe_count + 1
            
            ref entry = self._entries[index]
            
            if !entry.occupied:
                assert(false, "key not found")
            else if entry.hash == hash and compute_equal_of(entry.key, key):
                return entry.value
            else:
                let existing_entry_distance = (index + self._capacity - (entry.hash % self._capacity)) % self._capacity
                if existing_entry_distance < distance:
                    assert(false, "key not found")
                distance = distance + 1
                index = (index + 1) % self._capacity
        return self._entries[index].value
    
    fun contains(KeyType key) -> Bool:
        # Quick return for empty dictionary
        if self._size == 0:
            return false
        
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let probe_count = 0  # Add safety counter
        let to_return : Bool
        to_return = false

        while true:
            # Add safety check to prevent infinite loops
            if probe_count >= self._capacity:
                assert(false, "CONTAINS: Maximum probe count exceeded - likely an implementation bug")
            probe_count = probe_count + 1
            
            ref entry = self._entries[index]
            
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
        let probe_count = 0  # Add safety counter

        while true:
            # Add safety check to prevent infinite loops
            if probe_count >= self._capacity:
                assert(false, "REMOVE: Maximum probe count exceeded - likely an implementation bug")
                return false
            probe_count = probe_count + 1
            
            ref entry = self._entries[index]
            
            if !entry.occupied:
                break
            else if entry.hash == hash and compute_equal_of(entry.key, key):
                self._size = self._size - 1
                
                # Perform backward-shift operation
                let next_index = (index + 1) % self._capacity
                let current_index = index
                
                # Shift elements until we find an empty slot or an element with probe distance 0
                while true:
                    let next_entry = self._entries[next_index]
                    if !next_entry.occupied:
                        self._entries[current_index].occupied = false
                        break
                    
                    # Calculate probe distance of the next element
                    let next_probe_distance = (next_index + self._capacity - (next_entry.hash % self._capacity)) % self._capacity
                    
                    # If probe distance is 0, it's already at its ideal position
                    if next_probe_distance == 0:
                        self._entries[current_index].occupied = false
                        break
                    
                    # Move the element back
                    self._entries[current_index] = next_entry
                    
                    # Move to next positions
                    current_index = next_index
                    next_index = (next_index + 1) % self._capacity
                
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
        let counter = 0
        while counter < self._capacity:
            __builtin_destroy_do_not_use(self._entries[counter])
            counter = counter + 1
        __builtin_free_do_not_use(self._entries)

        self._capacity = 4
        self._size = 0
        self._entries = __builtin_malloc_do_not_use<Entry<KeyType, ValueType>>(self._capacity)
        let counter = 0
        while counter < self._capacity:
            self._entries[counter].occupied = false
            counter = counter + 1

    fun _grow():
        let old_capacity = self._capacity
        let old_entries = self._entries
        let old_size = self._size
        
        # Create new, larger entries array
        self._capacity = self._next_power_of_2(self._capacity + 1)  # Ensures growth by at least 1
        self._entries = __builtin_malloc_do_not_use<Entry<KeyType, ValueType>>(self._capacity)
        self._size = 0
        
        # Initialize new entries
        let counter = 0
        while counter < self._capacity:
            self._entries[counter].occupied = false
            counter = counter + 1

        # Copy old entries to new array, but only scan up to old_capacity
        counter = 0
        while counter < old_capacity:
            if old_entries[counter].occupied:
                # Insert directly without triggering another growth
                self._insert(self._entries, old_entries[counter].key, old_entries[counter].value)
            counter = counter + 1
        
        # Clean up old entries
        counter = 0
        while counter < old_capacity:
            __builtin_destroy_do_not_use(old_entries[counter])
            counter = counter + 1
        
        __builtin_free_do_not_use(old_entries)
        return

    fun drop():
        let counter = 0
        while counter < self._capacity:
            __builtin_destroy_do_not_use(self._entries[counter])
            counter = counter + 1
        if self._capacity != 0:
            __builtin_free_do_not_use(self._entries)
        self._size = 0
        self._capacity = 0

    # Private utility to find next power of 2
    fun _next_power_of_2(Int value) -> Int:
        let result = 1
        while result < value:
            result = result * 2
        return result
