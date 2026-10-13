# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls BigBox:
    Int value
    Int _secretValue
    SmallBox smallBox

cls SmallBox:
    Int value
    StringLiteral name

    fun init():
        self.value = 0
        self.name = "Unknown"

    fun multiplyValueBy2():
        self.value = self.value * 2

fun foo() ->OwningPtr<BigBox>:
    let ptr = __builtin_malloc_do_not_use<BigBox>(1)
     __builtin_construct_do_not_use(ptr[0])
    return ptr

fun main() ->Int:
    return 0

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Check if a pointer is correctly returned from a function

const ptr = $.foo();
assert.strictEqual(ptr.get().smallBox.name, "Unknown");

$.Std.free(ptr);
ptr._free();
$.Std.StringPool.free();

$.Std._detectMemoryLeaksDoNotUse();