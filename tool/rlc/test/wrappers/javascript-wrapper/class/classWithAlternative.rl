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

let strange = $.Cls_StrangeClass.create();
assert.strictEqual(strange.v_smallBox.v_value, 0);
assert.strictEqual(strange.v_smallBox.v_name, "Unknown");
assert.strictEqual(strange.v_alt.value, 0);
    
let alt = $.Alt3_Int_Cls_BigBox_StringLiteral.create("Hello");
strange.v_alt = alt;
assert.strictEqual(strange.v_alt.value, "Hello");

alt._free();
strange._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();