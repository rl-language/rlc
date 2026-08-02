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

cls StrangeClass:
    SmallBox smallBox
    Int | BigBox | StringLiteral alt

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if a class with an alternative as member field works

let strange = $.StrangeClass.create();
assert.strictEqual(strange.smallBox.value, 0);
assert.strictEqual(strange.smallBox.name, "Unknown");
assert.strictEqual(strange.alt.value, 0);
    
let alt = $.alt_int64_t_or_BigBox_or_strlit.create("Hello");
strange.alt = alt;
assert.strictEqual(strange.alt.value, "Hello");

alt._free();
strange._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();