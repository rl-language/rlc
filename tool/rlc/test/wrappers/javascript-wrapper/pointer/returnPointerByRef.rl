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

fun foo(SmartPointer obj) ->ref OwningPtr<Int>:
    return obj.ptr

fun main() ->Int:
    return 0

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if a pointer is correctly returned by ref

const obj = $.Cls_SmartPointer.create();
const ref = $.f_foo(obj);
assert.strictEqual(obj.v_ptr.get(), 5);
assert.strictEqual(ref.get(), 5);

ref.set(10);
assert.strictEqual(obj.v_ptr.get(), 10);
assert.strictEqual(ref.get(), 10);

obj._free();
$.StringPool.free();
$._detectMemoryLeaksDoNotUse();