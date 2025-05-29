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

const _max_load_factor = 0.75
const init_capacity = 4

cls<KeyType, ValueType> Dict_SoA:
    OwningPtr<KeyType> _keys
    OwningPtr<ValueType> _values
    OwningPtr<Int> _hashes
    Int _size
    Int _capacity
    
    fun init():
        self._capacity = init_capacity
        self._size = 0
        self._keys = __builtin_malloc_do_not_use<KeyType>(self._capacity)
        self._values = __builtin_malloc_do_not_use<ValueType>(self._capacity)
        self._hashes = __builtin_malloc_do_not_use<Int>(self._capacity)
        let counter = 0
        while counter < self._capacity:
            self._hashes[counter] = 0
            counter = counter + 1
    
    fun insert(KeyType key, ValueType value) -> Bool:
        let load_factor : Float
        load_factor = float(self._size + 1) / float(self._capacity)
        if load_factor > _max_load_factor:
            self._grow()
        self._insert(key, value)
        return true
    
    fun _insert(KeyType key, ValueType value):
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let probe_count = 0  # Add safety counter
        let stride = 16
        
        # Create local copies of key and value to avoid modifying the input parameters
        let current_key = key
        let current_value = value

        while probe_count < self._capacity:
            if self._hashes[index] == 0:
                __builtin_construct_do_not_use(self._keys[index])
                __builtin_construct_do_not_use(self._values[index])
                self._hashes[index] = hash
                self._keys[index] = current_key
                self._values[index] = current_value
                self._size = self._size + 1
                return

            let diff = self._capacity - index - stride
            if diff >= 0:
                let counter = 0
                let to_update = false
                let to_insert = false
                let to_substitute = false
                let acc_update = 0
                let acc_insert = 0
                let acc_substitute = 0
                while counter < stride :
                    to_insert = to_insert + (self._hashes[index + counter] == 0)
                    to_update = to_update + (self._hashes[index + counter] == hash)
                            * (compute_equal_of(self._keys[index + counter], current_key))
                    to_substitute = (((index + counter + self._capacity - (self._hashes[index + counter] % self._capacity)) % self._capacity)
                                    <= (distance + counter))
                    
                    if to_substitute:
                        acc_substitute = acc_substitute + 1
                    if to_insert:
                        acc_insert = acc_insert + 1
                    # acc_update = int(to_update) * (acc_update + 1)
                    if to_update :
                        acc_update = acc_update + 1
                        
                    counter = counter + 1
                
                
                # Insert
                if acc_insert > acc_update and
                    acc_insert >= acc_substitute:
                    index = stride - acc_insert
                    self._hashes[index] = hash
                    self._keys[index] = current_key
                    self._values[index] = current_value
                    self._size = self._size + 1
                    return

                # Update
                if acc_update > acc_substitute :
                    index = stride - acc_update
                    self._values[index] = current_value # Update the actual entry in entries
                    return
                
                distance = distance + stride
                # Substitute
                if to_substitute :
                    index = stride - acc_substitute
                    let tmp_hash = hash
                    let tmp_key = current_key
                    let tmp_val = current_value

                    current_value = self._values[index]
                    current_key = self._keys[index]
                    hash = self._hashes[index]
                    
                    self._values[index] = tmp_val 
                    self._keys[index] = tmp_key
                    self._hashes[index] = tmp_hash
                    distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    

                index = index + stride
                probe_count = probe_count + stride
                # distance = probe_count - 1
            else:
                while index < self._capacity:
                    if self._hashes[index] == 0:
                        __builtin_construct_do_not_use(self._keys[index])
                        __builtin_construct_do_not_use(self._values[index])
                        self._hashes[index] = hash
                        self._keys[index] = current_key
                        self._values[index] = current_value
                        self._size = self._size + 1
                        return
                    else if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
                        self._values[index] = current_value # Update the actual entry in entries
                        return
                    let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    if existing_entry_distance < distance:
                        let tmp_hash = hash
                        let tmp_key = current_key
                        let tmp_val = current_value

                        current_value = self._values[index]
                        current_key = self._keys[index]
                        hash = self._hashes[index]
                        
                        self._values[index] = tmp_val 
                        self._keys[index] = tmp_key
                        self._hashes[index] = tmp_hash
                        
                        distance = existing_entry_distance    
                    index = index + 1
                    distance = distance + 1
                index = 0
                probe_count = probe_count - (diff)
        assert(false, "Maximum probe count exceeded - likely an implementation bug")

    
    fun get(KeyType key) -> ValueType:
        # Quick return for empty dictionary
        if self._size == 0:
            assert(false, "key not found in empty dictionary")
        
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let probe_count = 0  # Add safety counter
        let stride = 16

        while probe_count < self._capacity:
            if self._hashes[index] == 0:
                assert(false, "key not found")

            let diff = self._capacity - index - stride
            if diff >= 0:
                let counter = 0
                let found = false
                let acc_found = 0
                while counter < stride :
                    found = found + (self._hashes[index + counter] == hash)
                            * (compute_equal_of(self._keys[index + counter], key))
                    counter = counter + 1
                    if found:
                        acc_found = acc_found + 1
                if found :
                    index = stride - acc_found
                    return self._values[index]

                let last_distance = (index + stride + self._capacity - (self._hashes[index + stride] % self._capacity)) % self._capacity
                probe_count = probe_count + stride
                if last_distance < probe_count - 1:
                    assert(false, "key not found")
                index = index + stride
            else:
                while index < self._capacity:
                    if self._hashes[index] == 0:
                        assert(false, "key not found")
                    if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
                        return self._values[index]
                    let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    if existing_entry_distance < probe_count:
                        assert(false, "key not found")
                    index = index + 1
                    probe_count = probe_count + 1
                index = 0

        assert(false, "GET: Maximum probe count exceeded - likely an implementation bug")
    
    fun contains(KeyType key) -> Bool:
        # Quick return for empty dictionary
        if self._size == 0:
            return false
        
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let start_index = index
        let stride = 16
        let probe_count = 0  # Add safety counter

        while probe_count < self._capacity:
            if self._hashes[index] == 0:
                return false
            let diff = self._capacity - index - stride
            if diff >= 0:
                let counter = 0
                let sum = false
                while counter < stride :
                    sum = sum + (self._hashes[index + counter] == hash)
                            * (compute_equal_of(self._keys[index + counter], key))
                    counter = counter + 1
                if sum :
                    return true
                let last_distance = (index + stride + self._capacity - (self._hashes[index + stride] % self._capacity)) % self._capacity
                probe_count = probe_count + stride
                if last_distance < probe_count - 1:
                    return false
                index = index + stride
            else:
                while index < self._capacity:
                    if self._hashes[index] == 0:
                        return false
                    if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
                        return true
                    let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    if existing_entry_distance < probe_count:
                        return false
                    index = index + 1
                    probe_count = probe_count + 1
                index = 0

        assert(false, "CONTAINS: Maximum probe count exceeded - likely an implementation bug")

    fun remove(KeyType key) -> Bool:
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let distance = 0
        let probe_count = 0  # Add safety counter

        while probe_count < self._capacity:
            if self._hashes[index] == 0:
                return false

            let diff = self._capacity - index - stride
            if diff >= 0:
                let counter = 0
                let to_update = false
                let to_insert = false
                let to_substitute = false
                let acc_update = 0
                let acc_insert = 0
                let acc_substitute = 0
                while counter < stride :
                    to_insert = to_insert + (self._hashes[index + counter] == 0)
                    to_update = to_update + (self._hashes[index + counter] == hash)
                            * (compute_equal_of(self._keys[index + counter], current_key))
                    to_substitute = (((index + counter + self._capacity - (self._hashes[index + counter] % self._capacity)) % self._capacity)
                                    <= (distance + counter))
                    
                    if to_substitute:
                        acc_substitute = acc_substitute + 1
                    if to_insert:
                        acc_insert = acc_insert + 1
                    # acc_update = int(to_update) * (acc_update + 1)
                    if to_update :
                        acc_update = acc_update + 1
                        
                    counter = counter + 1
                
                
                # Insert
                if acc_insert > acc_update and
                    acc_insert >= acc_substitute:
                    index = stride - acc_insert
                    self._hashes[index] = hash
                    self._keys[index] = current_key
                    self._values[index] = current_value
                    self._size = self._size + 1
                    return

                # Update
                if acc_update > acc_substitute :
                    index = stride - acc_update
                    self._values[index] = current_value # Update the actual entry in entries
                    return
                
                distance = distance + stride
                # Substitute
                if to_substitute :
                    index = stride - acc_substitute
                    let tmp_hash = hash
                    let tmp_key = current_key
                    let tmp_val = current_value

                    current_value = self._values[index]
                    current_key = self._keys[index]
                    hash = self._hashes[index]
                    
                    self._values[index] = tmp_val 
                    self._keys[index] = tmp_key
                    self._hashes[index] = tmp_hash
                    distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    

                index = index + stride
                probe_count = probe_count + stride
                # distance = probe_count - 1
            else:
                while index < self._capacity:
                    if self._hashes[index] == 0:
                        return false
                    if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
                        self._size = self._size - 1
                        
                        # Perform backward-shift operation
                        let next_index = (index + 1) % self._capacity
                        let current_index = index
                        
                        # Shift elements until we find an empty slot or an element with probe distance 0
                        while true:
                            let next_hash = self._hashes[next_index]
                            let next_key = self._keys[next_index]
                            let next_val = self._values[next_index]
                            
                            if next_hash == 0:
                                self._hashes[current_index] = 0
                                break
                            
                            # Calculate probe distance of the next element
                            let next_probe_distance = (next_index + self._capacity - (next_hash % self._capacity)) % self._capacity
                            
                            # If probe distance is 0, it's already at its ideal position
                            if next_probe_distance == 0:
                                self._hashes[current_index] = 0
                                break
                            
                            # Move the element back
                            self._keys[current_index] = next_key
                            self._values[current_index] = next_val
                            self._hashes[current_index] = next_hash
                            
                            # Move to next positions
                            current_index = next_index
                            next_index = (next_index + 1) % self._capacity
                        
                        return true
                    
                    let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    if existing_entry_distance < distance:
                        return false
                    distance = distance + 1
                    index = index + 1
                index = 0
                probe_count = probe_count - (diff)
        assert(false, "REMOVE: Maximum probe count exceeded - likely an implementation bug")

    
    fun keys() -> Vector<KeyType>:
        let to_return : Vector<KeyType>
        to_return.resize(self.size)
        let counter = 0
        let index = 0
        while counter < self._size:
            if self._hashes[index] != 0:
                to_return.append(self._keys[index])
                counter = counter + 1
            index = index + 1
        return to_return

    fun values() -> Vector<ValueType>:
        let to_return : Vector<ValueType>
        to_return.resize(self.size)
        let counter = 0
        let index = 0
        while counter < self._size:
            if self._hashes[index] != 0:
                to_return.append(self._values[index])
                counter = counter + 1
            index = index + 1
        return to_return

    # returns true if the
    # size of the dictionary is equal
    # to zero
    fun empty() -> Bool:
        return self._size == 0
    
    fun size() -> Int:
        return self._size

    # erases all the elements
    # of the dictionary
    fun clear():
        let counter = 0
        while counter < self._capacity:
            __builtin_destroy_do_not_use(self._keys[counter])
            __builtin_destroy_do_not_use(self._values[counter])
            counter = counter + 1
        __builtin_free_do_not_use(self._values)
        __builtin_free_do_not_use(self._keys)
        __builtin_free_do_not_use(self._hashes)

        self.init()

    fun _grow():
        if self._capacity == 0:
            self._capacity = 1
            self._keys = __builtin_malloc_do_not_use<KeyType>(self._capacity)
            self._values = __builtin_malloc_do_not_use<ValueType>(self._capacity)
            self._hashes = __builtin_malloc_do_not_use<Int>(self._capacity) 
            self._hashes[0] = 0
            return
        
        if self._capacity == 1:
            let old_hashes = self._hashes
            let old_values = self._values
            let old_keys = self._keys

            # Create new, larger entries array
            self._capacity = 2
            self._keys = __builtin_malloc_do_not_use<KeyType>(self._capacity)
            self._values = __builtin_malloc_do_not_use<ValueType>(self._capacity)
            self._hashes = __builtin_malloc_do_not_use<Int>(self._capacity)
            self._size = 0
            # Initialize new entries
            self._hashes[0] = 0
            self._hashes[1] = 0
            # Copy old entries to new array
            # Insert directly without triggering another growth
            self._insert(old_keys[0], old_values[0])
            
            # Clean up old entries
            __builtin_destroy_do_not_use(old_values[0])
            __builtin_destroy_do_not_use(old_keys[0])
            __builtin_free_do_not_use(old_keys)
            __builtin_free_do_not_use(old_values)
            __builtin_free_do_not_use(old_hashes)
            return
            
        let old_capacity = self._capacity
        let old_hashes = self._hashes
        let old_values = self._values
        let old_keys = self._keys
        let old_size = self._size
        
        # Create new, larger array
        self._capacity = self._capacity << 1
        self._keys = __builtin_malloc_do_not_use<KeyType>(self._capacity)
        self._values = __builtin_malloc_do_not_use<ValueType>(self._capacity)
        self._hashes = __builtin_malloc_do_not_use<Int>(self._capacity)
        self._size = 0
        
        # Initialize new entries
        let counter = 0
        while counter < self._capacity:
            self._hashes[counter] = 0
            counter = counter + 1

        # Copy old entries to new array, but only scan up to old_capacity
        counter = 0
        while counter < old_capacity:
            if old_hashes[counter] != 0:
                # Insert directly without triggering another growth
                self._insert(old_keys[counter], old_values[counter])
            counter = counter + 1
        
        # Clean up old entries
        counter = 0
        while counter < old_capacity:
            __builtin_destroy_do_not_use(old_keys[counter])
            __builtin_destroy_do_not_use(old_values[counter])
            counter = counter + 1
        
        __builtin_free_do_not_use(old_keys)
        __builtin_free_do_not_use(old_values)
        __builtin_free_do_not_use(old_hashes)
        return

    fun drop():
        let counter = 0
        while counter < self._capacity:
            __builtin_destroy_do_not_use(self._keys[counter])
            __builtin_destroy_do_not_use(self._values[counter])
            counter = counter + 1
        if self._capacity != 0:
            __builtin_free_do_not_use(self._values)
            __builtin_free_do_not_use(self._keys)
            __builtin_free_do_not_use(self._hashes)
        self._size = 0
        self._capacity = 0

    # Private utility to find next power of 2
    fun _next_power_of_2(Int value) -> Int:
        let result = 1
        while result < value:
            result = result * 2
        return result
