# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls SmartPointer:
    OwningPtr<Int> ptr

    fun init():
        self.ptr = __builtin_malloc_do_not_use<Int>(1)
        __builtin_construct_do_not_use(self.ptr[0])
        self.ptr[0] = 5

    fun drop():
         __builtin_destroy_do_not_use(self.ptr[0])
         __builtin_free_do_not_use(self.ptr)

cls ArraySmartPointer:
    OwningPtr<SmartPointer> ptr

    fun init():
        self.ptr = __builtin_malloc_do_not_use<SmartPointer>(3)
        __builtin_construct_do_not_use(self.ptr[0])
        __builtin_construct_do_not_use(self.ptr[1])
        __builtin_construct_do_not_use(self.ptr[2])


    fun drop():
         __builtin_destroy_do_not_use(self.ptr[0])
         __builtin_destroy_do_not_use(self.ptr[1])
         __builtin_destroy_do_not_use(self.ptr[2])
         __builtin_free_do_not_use(self.ptr)

fun foo(Int | SmartPointer x, SmartPointer[2] y):
    let z : SmartPointer[2]
    x = 2

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if drop functions are correctly called to free up memory



//ClassWithPointer
const obj1 = $.SmartPointer.create();

obj1._free();



//ClassWithPointerAsArray
const obj2 = $.ArraySmartPointer.create();
for (let i = 0; i < 3; i++) {
    assert.strict(obj2.ptr.get(i).ptr.get(), 5);
}

obj2._free();



//Array of objects with pointers
const arr = $.SmartPointer_2._createUninitialized();
arr.get(0).ptr.value = $.ptr_Int.calloc();
arr.get(1).ptr.value = $.ptr_Int.calloc();

arr.get(0).ptr.set(4);
arr.get(1).ptr.set(8);

assert.strictEqual(arr.get(0).ptr.get(), 4);
assert.strictEqual(arr.get(1).ptr.get(), 8);

arr._free();



//Alternative with pointers
const alt = $.alt_int64_t_or_SmartPointer._createUninitialized();
assert.strictEqual(alt._index, -1);

alt.value = 8;
assert.strictEqual(alt.value, 8);

alt._free();



$.StringPool.free();
$._detectMemoryLeaksDoNotUse();