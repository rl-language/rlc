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

//Checking that we can assign values to the alternative

let smallBox = $.SmallBox.create({v_value: 5, v_name: "Hello"});
let bigBox = $.BigBox.create({v_value: 12, v_smallBox: smallBox});

let alternative = $.Alt3_Int_BigBox_StringLiteral.create(5);
assert.strictEqual(alternative.value, 5);

alternative.value = 9;
assert.strictEqual(alternative.value, 9);

alternative.value = bigBox;
assert.strictEqual(alternative.value.v_value, 12);
assert.strictEqual(alternative.value.v_smallBox.v_value, 5);
assert.strictEqual(alternative.value.v_smallBox.v_name, "Hello");

alternative.value = "This is a string";
assert.strictEqual(alternative.value, "This is a string");

smallBox._free();
bigBox._free();
alternative._free();
    
$.StringPool.free();
$._detectMemoryLeaksDoNotUse();