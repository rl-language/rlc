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

fun foo(BigBox bigBox) -> SmallBox:
    return bigBox.smallBox

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if objects are correctly returned by functions

let bigBox = $.BigBox.create();
let smallBox = $.foo(bigBox);

smallBox.name = "Name";
assert.strictEqual(smallBox.name, "Name");
assert.strictEqual(bigBox.smallBox.name, "Unknown");

bigBox.smallBox.name = "Other name";
assert.strictEqual(bigBox.smallBox.name, "Other name")
assert.strictEqual(smallBox.name, "Name");

bigBox._free();
smallBox._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();