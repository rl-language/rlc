## Copyright 2024 Massimo Fioravanti
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


# A memory contiguous datastructure similar to cpp vector.
# A vector is a list of elements. Just like cpp vector,
# the contents may be reallocated when added or deleated,
# so references elements are invalidated if the 
# vector is modified.
cls<T> Vector:
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

    # changes the size of the vector
    # to be equal to `new_size`
    # if the original size is larger
    # than the new size, the extra 
    # elements are destroyed
    # 
    # if the original size is smaller
    # than the new size, extra elements
    # are constructed
    fun resize(Int new_size):
        if new_size > self._size:
          self._grow(new_size)
          let x : T
          while self._size != new_size:
            self.append(x)
        else:
          while self._size > new_size:
            self.pop()

    # returns a reference to the
    # last element of the vector
    fun back() -> ref T:
        return self._data[self._size - 1]

    # returns a reference to the element
    # with the provided index
    fun get(Int index) -> ref T:
        return self._data[index]

    # assigns `value` to the element
    # of the vector at the provided
    # index
    fun set(Int index, T value):
        self._data[index] = value

    # appends `value` to the 
    # end of the vector
    fun append(T value):
        self._grow(self._size + 1)
        self._data[self._size] = value
        self._size = self._size + 1

    # returns true if the
    # size of the vector is equal
    # to zero
    fun empty() -> Bool:
        return self._size == 0

    # erases all the elements
    # of the vector
    fun clear():
        while !self.empty():
            self.pop()

    # removes the last element
    # of the vector and returns
    # it by copy
    fun pop() -> T:
        let to_return = self._data[self._size - 1]
        self._size = self._size - 1
        __builtin_destroy_do_not_use(self._data[self._size])
        __builtin_construct_do_not_use(self._data[self._size])
        return to_return

    # removes `quantity` elements
    # from the back of the vector
    fun drop_back(Int quantity):
        let counter = self._size - quantity
        while counter < self._size: 
            __builtin_destroy_do_not_use(self._data[counter])
            __builtin_construct_do_not_use(self._data[counter])
            counter = counter + 1
        self._size = self._size - quantity

    # erase the element with the provided
    # `index`
    fun erase(Int index):
        let counter = index
        while counter < self._size - 1: 
            self._data[counter] = self._data[counter + 1]
            counter = counter + 1
        self.pop()  

    fun size() -> Int:
        return self._size

# A bounded vector is a wrapper
# around a vector that prevents 
# the size of the vector to ever 
# exced `max_size`
# this class is usefull when used
# for machine learning techniques, 
# since it allows the machine learning
# to know the maximal possible size
# of this vector when converted to
# a tensor
cls<T, Int max_size> BoundedVector:
    Vector<T> _data

    fun assign(BoundedVector<T, max_size> other):
        self._data = other._data

    # identical to Vector::resize,
    # except `new_size` is clamped
    # to be at most `max_size`
    fun resize(Int new_size):
        if new_size > max_size:
            new_size = max_size
        self._data.resize(new_size)

    # returns the maximal possible
    # size of this vector
    fun max_size() -> Int:
        return max_size

    # same as vector::back
    fun back() -> ref T:
        return self._data.back()

    # same as vector::get
    fun get(Int index) -> ref T:
        return self._data.get(index)

    # same as vector::set
    fun set(Int index, T value):
        self._data.set(index, value)

    # append `value` to the end
    # of the vector, but only if
    # doing so would not excede
    # max vector maximal size
    fun append(T value): 
        if max_size > self.size():
          self._data.append(value)

    # same as vector::empty
    fun empty() -> Bool:
        return self._data.empty()

    # same as vector::clear
    fun clear():
        self._data.clear()

    # same as vector::pop
    fun pop() -> T:
        return self._data.pop()

    # same as vector::drop_back
    fun drop_back(Int quantity):
        self._data.drop_back(quantity)

    # same as vector::erase
    fun erase(Int index):
        self._data.erase(index)

    # same as vector::size
    fun size() -> Int:
        return self._data.size()
