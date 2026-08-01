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

fun foo(Int | BigBox | StringLiteral alt):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if alternatives are cloned correctly

const original = $.Alt3_Int_BigBox_StringLiteral.create();
original.value = 8;
const cloned = $.Alt3_Int_BigBox_StringLiteral.clone(original);
assert.strictEqual(original.value, cloned.value);
    
cloned.value = "Hello";
assert.strictEqual(cloned.value, "Hello");
assert.strictEqual(original.value, 8);
    
original._free();
cloned._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();