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
    alt = x

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that we can assign values to the alternative

let smallBox = $.SmallBox.create({value: 5, name: "Hello"});
let bigBox = $.BigBox.create({value: 12, smallBox: smallBox});

let alternative = $.alt_int64_t_or_BigBox_or_strlit.create(5);
assert.strictEqual(alternative.value, 5);

alternative.value = 9;
assert.strictEqual(alternative.value, 9);

alternative.value = bigBox;
assert.strictEqual(alternative.value.value, 12);
assert.strictEqual(alternative.value.smallBox.value, 5);
assert.strictEqual(alternative.value.smallBox.name, "Hello");

alternative.value = "This is a string";
assert.strictEqual(alternative.value, "This is a string");

smallBox._free();
bigBox._free();
alternative._free();
    
$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();