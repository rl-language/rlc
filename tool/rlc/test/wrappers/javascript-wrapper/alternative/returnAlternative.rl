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

fun foo(Int | BigBox | StringLiteral alt) -> Int | BigBox | StringLiteral:
    alt = "Changed by Rulebook"
    return alt

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if alternatives are correctly returned by functions

let alternative = $.alt_int64_t_or_BigBox_or_strlit.create();
assert.strictEqual(alternative.value, 0);

const result = $.foo(alternative);
assert.strictEqual(alternative.value, "Changed by Rulebook");
assert.strictEqual(result.value, "Changed by Rulebook");
assert.notStrictEqual(result._address, alternative._address);
    
alternative._free();
result._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();