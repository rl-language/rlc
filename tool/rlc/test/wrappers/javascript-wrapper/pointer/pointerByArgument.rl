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

fun foo(OwningPtr<BigBox> ptr):
    ptr[0].smallBox.name = "Hello"

#--- test.mjs

import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if pointers are correctly passed as function arguments

const ptr = $.ptr_BigBox.create($.ptr_BigBox.calloc());
assert.strictEqual(ptr.get().smallBox.name, "Unknown");

$.foo(ptr);
assert.strictEqual(ptr.get().smallBox.name, "Hello");

$.free(ptr);
ptr._free();
$.StringPool.free();

$._detectMemoryLeaksDoNotUse();