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

fun foo(SmallBox smallBox, BigBox bigBox):
    let otherSmallBox : SmallBox
    let otherBigBox : BigBox

    otherSmallBox.name = "Other small box"
    otherBigBox.smallBox.name = "Small box inside big box"
    
    smallBox = otherSmallBox
    bigBox = otherBigBox

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if objects are properly passed as function arguments

let smallBox = $.Cls_SmallBox.create();
let bigBox = $.Cls_BigBox.create();

$.f_foo(smallBox, bigBox);
assert.strictEqual(smallBox.v_name, "Other small box");
assert.strict(bigBox.v_smallBox.v_name, "Small box inside big box");

smallBox._free();
bigBox._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();