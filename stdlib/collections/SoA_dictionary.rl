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
const _init_capacity = 4
const _stride = 17

cls<KeyType, ValueType> Dict_SoA:
    OwningPtr<KeyType> _keys
    OwningPtr<ValueType> _values
    OwningPtr<Int> _hashes
    Int _size
    Int _capacity
    
    fun init():
        self._capacity = _init_capacity
        self._size = 0
        self._keys = __builtin_malloc_do_not_use<KeyType>(self._capacity)
        self._values = __builtin_malloc_do_not_use<ValueType>(self._capacity)
        self._hashes = __builtin_malloc_do_not_use<Int>(self._capacity)
        let counter = 0
        while counter < _init_capacity:
            self._hashes[counter] = -1
            counter = counter + 1
    
    fun insert(KeyType key, ValueType value) -> Bool:
        # if called after drop()
        if self._capacity == 0:
            self.init()
        else:    
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
        
        # Create local copies of key and value to avoid modifying the input parameters
        let current_key = key
        let current_value = value

        while probe_count < self._capacity:
            if self._hashes[index] == -1:
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
            if index == self._capacity:
                index = 0
            distance = distance + 1
            probe_count = probe_count + 1
        assert(false, "Maximum probe count exceeded - likely an implementation bug")
        return
        
    fun get(KeyType key) -> ValueType:
        # Quick return for empty dictionary
        if self._size == 0:
            assert(false, "key not found in empty dictionary")
        
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let probe_count = 0  # Add safety counter

        # STANDARD IMPL
        # while probe_count < self._capacity:
        #     if self._hashes[index] == -1:
        #         assert(false, "key not found")
        #     if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
        #         return self._values[index]
        #     let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
        #     if existing_entry_distance < probe_count:
        #         assert(false, "key not found")
        #     index = index + 1
        #     probe_count = probe_count + 1
            
        #     if index >= self._capacity:
        #         index = 0

        if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
            return self._values[index]

        while probe_count < self._capacity:
            if self._hashes[index] == -1:
                assert(false, "key not found")

            let diff = self._capacity - index - _stride
            # TODO: put a size threshold here
            if diff >= 0:
                let counter = 0
                let sum = false
                while counter < _stride :
                    sum = sum + (self._hashes[index + counter] == hash) * (compute_equal_of(self._keys[index + counter], key))
                    counter = counter + 1
                if sum :
                    counter = 0
                    while counter < _stride :
                        if self._hashes[index + counter] == hash and compute_equal_of(self._keys[index + counter], key):
                            return self._values[index]
                        counter = counter + 1

                let last_distance = (index + _stride + self._capacity - (self._hashes[index + _stride] % self._capacity)) % self._capacity
                probe_count = probe_count + _stride
                if last_distance < probe_count - 1:
                    assert(false, "key not found")
                index = index + _stride
            else:
                while index < self._capacity:
                    if self._hashes[index] == -1:
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
        return self._values[0]
    
    fun contains(KeyType key) -> Bool:
        # Quick return for empty dictionary
        if self._size == 0:
            return false
        
        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let start_index = index
        let probe_count = 0  # Add safety counter

        while probe_count < self._capacity:
            if self._hashes[index] == -1:
                return false
            let diff = self._capacity - index - _stride
            if diff >= 0:
                if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
                    return true
                let counter = 0
                let sum = false
                while counter < _stride :
                    sum = sum + (self._hashes[index + counter] == hash) * (compute_equal_of(self._keys[index + counter], key))
                    counter = counter + 1
                if sum :
                    return true
                index = index + _stride
                probe_count = probe_count + _stride
                let last_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                if last_distance < probe_count - 1:
                    return false
            else:
                while index < self._capacity:
                    if self._hashes[index] == -1:
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
        return false

    fun _shift_back(Int index):
        self._size = self._size - 1
                        
        # Perform backward-shift operation
        let next_index = (index + 1) % self._capacity
        let current_index = index
        
        # Shift elements until we find an empty slot or an element with probe distance 0
        while true:
            let next_hash = self._hashes[next_index]
            let next_key = self._keys[next_index]
            let next_val = self._values[next_index]
            
            if next_hash == -1:
                self._hashes[current_index] = -1
                return
            
            # Calculate probe distance of the next element
            let next_probe_distance = (next_index + self._capacity - (next_hash % self._capacity)) % self._capacity
            
            # If probe distance is 0, it's already at its ideal position
            if next_probe_distance == 0:
                self._hashes[current_index] = -1
                return
            
            # Move the element back
            self._keys[current_index] = next_key
            self._values[current_index] = next_val
            self._hashes[current_index] = next_hash
            
            # Move to next positions
            current_index = next_index
            next_index = (next_index + 1) % self._capacity
        return

    fun remove(KeyType key) -> Bool:
        if self._size == 0:
            return false

        let hash = compute_hash_of(key)
        let index = hash % self._capacity
        let probe_count = 0  # Add safety counter

        # STANDARD IMPL
        # while probe_count < self._capacity:
        #     if self._hashes[index] == -1:
        #         return false
        #     if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
        #         self._shift_back(index)
        #         return true
            
        #     let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
        #     if existing_entry_distance < probe_count:
        #         return false
        #     index = index + 1
        #     probe_count = probe_count + 1

        #     if index >= self._capacity:
        #         index = 0
        
        if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
            self._shift_back(index)
            return true

        while probe_count < self._capacity:
            if self._hashes[index] == -1:
                return false

            let diff = self._capacity - index - _stride
            # TODO: put a size threshold here
            if diff >= 0:
                let counter = 0
                let sum = false
                while counter < _stride :
                    sum = sum + (self._hashes[index + counter] == hash) * (compute_equal_of(self._keys[index + counter], key))
                    counter = counter + 1
                if sum :
                    counter = 0
                    while counter < _stride :
                        if self._hashes[index + counter] == hash and compute_equal_of(self._keys[index + counter], key):
                            self._shift_back(index + counter)
                            return true
                        counter = counter + 1
                    assert(false, "Error: Reached the end of getting index loop of a found element")

                index = index + _stride
                probe_count = probe_count + _stride
                let last_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                if last_distance < probe_count - 1:
                    return false
            else:
                while index < self._capacity:
                    if self._hashes[index] == -1:
                        return false
                    if self._hashes[index] == hash and compute_equal_of(self._keys[index], key):
                        self._shift_back(index)
                        return true
                    
                    let existing_entry_distance = (index + self._capacity - (self._hashes[index] % self._capacity)) % self._capacity
                    if existing_entry_distance < probe_count:
                        return false
                    index = index + 1
                    probe_count = probe_count + 1
                index = 0

        assert(false, "REMOVE: Maximum probe count exceeded - likely an implementation bug")
        return false

    
    fun keys() -> Vector<KeyType>:
        let to_return : Vector<KeyType>
        to_return.resize(self._size)
        let counter = 0
        let index = 0
        while counter < self._size:
            if self._hashes[index] != -1:
                to_return.set(counter, self._keys[index])
                counter = counter + 1
            index = index + 1
        return to_return

    fun values() -> Vector<ValueType>:
        let to_return : Vector<ValueType>
        to_return.resize(self._size)
        let counter = 0
        let index = 0
        while counter < self._size:
            if self._hashes[index] != -1:
                to_return.set(counter, self._values[index])
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

    fun _clear():
        let counter = 0
        while counter < self._capacity:
            __builtin_destroy_do_not_use(self._keys[counter])
            __builtin_destroy_do_not_use(self._values[counter])
            counter = counter + 1
        if self._capacity != 0:
            __builtin_free_do_not_use(self._values)
            __builtin_free_do_not_use(self._keys)
            __builtin_free_do_not_use(self._hashes)

    # erases all the elements
    # of the dictionary
    fun clear():
        self._clear()
        self.init()

    fun _grow():
        let old_capacity = self._capacity
        let old_hashes = self._hashes
        let old_values = self._values
        let old_keys = self._keys
        
        # Create new, larger array
        self._capacity = (self._capacity << 1) #+ 1
        self._keys = __builtin_malloc_do_not_use<KeyType>(self._capacity)
        self._values = __builtin_malloc_do_not_use<ValueType>(self._capacity)
        self._hashes = __builtin_malloc_do_not_use<Int>(self._capacity)
        self._size = 0
        
        # Initialize new entries
        let counter = 0
        while counter < self._capacity:
            self._hashes[counter] = -1
            counter = counter + 1

        # Copy old entries to new array, but only scan up to old_capacity
        counter = 0
        while counter < old_capacity:
            if old_hashes[counter] != -1:
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
        self._clear()
        self._size = 0
        self._capacity = 0

    fun print_dict():
        let counter = 0
        print("Dictionary")
        print("Size = "s + to_string(self._size) + ", Capacity = "s + to_string(self._capacity))
        print("Index | Hash | Value | Key")
        while counter < self._capacity:
            if self._hashes[counter] != -1:
                print(to_string(counter) + " | "s + 
                    to_string(self._hashes[counter]) + " | "s +
                    to_string(self._values[counter]) + " | "s +
                    to_string(self._keys[counter]))
            counter = counter + 1