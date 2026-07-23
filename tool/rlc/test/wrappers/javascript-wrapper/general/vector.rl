# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
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
            counter = counter + 1

        counter = 0
        while counter < self._capacity:
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
        assert(self._size > 0, "out of bound vector access")
        return self._data[self._size - 1]

    # returns a reference to the element
    # with the provided index
    fun get(Int index) -> ref T:
        assert(index >= 0, "out of bound vector access")
        assert(index < self._size, "out of bound vector access")
        return self._data[index]

    # assigns `value` to the element
    # of the vector at the provided
    # index
    fun set(Int index, T value):
        assert(index >= 0, "out of bound vector access")
        assert(index < self._size, "out of bound vector access")
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
        assert(self._size > 0, "out of bound vector access")
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
        assert(index >= 0, "out of bound vector access")
        assert(index < self._size, "out of bound vector access")
        let counter = index
        while counter < self._size - 1: 
            self._data[counter] = self._data[counter + 1]
            counter = counter + 1
        self.pop()  

    fun size() -> Int:
        return self._size


fun foo(Vector<Int> vector):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the Vector class of the standard library works

const vector = $.Cls_Vector.create();
for(let i=0; i<100; i++){
    vector.f_append(i);
}

for(let i=0; i<100; i++){
    assert.strictEqual(vector.f_get(i).value, i);
}

assert.strictEqual(vector.v__data, undefined);
assert.strictEqual(vector.v__size, undefined);
assert.strictEqual(vector.v__capacity, undefined);



vector._free();
$._detectMemoryLeaksDoNotUse();