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

using Alt = Int | BigBox | StringLiteral
fun foo(Alt alt1, BigBox | Alt alt2):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the default initialization works for alternatives

let alt = $.Alt3_Int_Cls_BigBox_StringLiteral.create();
assert.strictEqual(alt.value, 0);
    
alt.value = "Hello";
assert.strictEqual(alt.value, "Hello");

alt._free();

alt = $.Alt2_Cls_BigBox_Alt3_Int_Cls_BigBox_StringLiteral.create();
assert.strictEqual(alt.value instanceof $.Cls_BigBox, true);

alt._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();