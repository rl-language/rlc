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

const ptr = $.Ptr_Cls_BigBox.create($.Ptr_Cls_BigBox.malloc());
assert.strictEqual(ptr.get().v_smallBox.v_name, "Unknown");

$.f_foo(ptr);
assert.strictEqual(ptr.get().v_smallBox.v_name, "Hello");

$.free(ptr);
ptr._free();
$.StringPool.free();

$._detectMemoryLeaksDoNotUse();