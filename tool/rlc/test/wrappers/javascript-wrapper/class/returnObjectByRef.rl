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

fun foo(BigBox bigBox) -> ref SmallBox:
    return bigBox.smallBox

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if objects are correctly returned by ref

let bigBox = $.Cls_BigBox.create();
let smallBox = $.f_foo(bigBox);

smallBox.v_name = "Name";
assert.strictEqual(bigBox.v_smallBox.v_name, "Name");

bigBox.v_smallBox.v_name = "Other name";
assert.strictEqual(smallBox.v_name, "Other name")

bigBox._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();