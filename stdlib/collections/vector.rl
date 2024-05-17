# Copyright 2024 Massimo Fioravanti
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

ent<T> Vector:
    OwningPtr<T> _data
    Int _size
    Int _capacity

    fun _grow(Int target_size):
        if self._capacity > target_size:
            return

        let new_data = __builtin_malloc_do_not_use<T>(target_size * 2)
        let counter = 0
        while counter < target_size * 2:
            __builtin_construct_do_not_use(new_data[counter])
            counter = counter + 1

        counter = 0
        while counter < self._size:
            new_data[counter] = self._data[counter] 
            __builtin_destroy_do_not_use(self._data[counter])
            counter = counter + 1

        __builtin_free_do_not_use(self._data)
        self._capacity = target_size * 2
        self._data = new_data

    fun init():
        self._size = 0
        self._capacity = 4
        self._data = __builtin_malloc_do_not_use<T>(4)
        let counter = 0
        while counter < self._capacity:
            __builtin_construct_do_not_use(self._data[counter])
            counter = counter + 1

    fun drop():
        let counter = 0
        while counter != self._capacity:
            __builtin_destroy_do_not_use(self._data[counter])
            counter = counter + 1
        if self._capacity != 0:
          __builtin_free_do_not_use(self._data)
        self._size = 0
        self._capacity = 0

    fun assign(Vector<T> other):
        self.drop()
        self.init()
        let counter = 0
        while counter < other._size:
            self.append(other.get(counter))
            counter = counter + 1

    fun resize(Int new_size):
        if new_size > self._size:
          self._grow(new_size)
          let x : T
          while self._size != new_size:
            self.append(x)
        else:
          while self._size > new_size:
            self.pop()

    fun back() -> ref T:
        return self._data[self._size - 1]

    fun get(Int index) -> ref T:
        return self._data[index]

    fun set(Int index, T value):
        self._data[index] = value

    fun append(T value):
        self._grow(self._size + 1)
        self._data[self._size] = value
        self._size = self._size + 1

    fun empty() -> Bool:
        return self._size == 0

    fun clear():
        while !self.empty():
            self.pop()

    fun pop() -> T:
        let to_return = self._data[self._size - 1]
        self._size = self._size - 1
        __builtin_destroy_do_not_use(self._data[self._size])
        __builtin_construct_do_not_use(self._data[self._size])
        return to_return

    fun drop_back(Int quantity):
        let counter = self._size - quantity
        while counter < self._size: 
            __builtin_destroy_do_not_use(self._data[counter])
            __builtin_construct_do_not_use(self._data[counter])
            counter = counter + 1
        self._size = self._size - quantity

    fun erase(Int index):
        let counter = index
        while counter < self._size - 1: 
            self._data[counter] = self._data[counter + 1]
            counter = counter + 1
        self.pop()  

    fun size() -> Int:
        return self._size
