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
fun foo1(Alt alt):
    1+1

fun foo2(BigBox | Alt alt):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that alternatives containing alternatives work

let bigBox = $.BigBox.create();
let altWithAlt = $.Alt2_BigBox_Alt3_Int_BigBox_StringLiteral.create(bigBox);
assert.strictEqual(altWithAlt.value.v_value, 0);
assert.strictEqual(altWithAlt.value.v_smallBox.v_value, 0);
assert.strictEqual(altWithAlt.value.v_smallBox.v_name, "Unknown");

let alt = $.Alt3_Int_BigBox_StringLiteral.create("Hello");
altWithAlt.value = alt;
assert.strictEqual(altWithAlt.value.value, "Hello");
    
bigBox._free();
alt._free();
altWithAlt._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();