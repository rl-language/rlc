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
    alt = "Changed by Rulebook"

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that alternatives are correctly passed as function arguments

let alternative = $.Alt3_Int_BigBox_StringLiteral.create("Hello");
assert.strictEqual(alternative.value, "Hello");

$.f_foo(alternative);
assert.strictEqual(alternative.value, "Changed by Rulebook");
    
alternative._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();