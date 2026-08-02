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
    let x : Int | BigBox | StringLiteral
    alt = "Changed by Rulebook"

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that alternatives are correctly passed as function arguments

let alternative = $.alt_int64_t_or_BigBox_or_strlit.create("Hello");
assert.strictEqual(alternative.value, "Hello");

$.foo(alternative);
assert.strictEqual(alternative.value, "Changed by Rulebook");
    
alternative._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();