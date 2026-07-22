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

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if I can assign objects to object fields

let smallBox = $.Cls_SmallBox.create({v_value: 50, v_name: "Gino"});
let bigBox = $.Cls_BigBox.create();
bigBox.v_smallBox = smallBox;
assert.strictEqual(bigBox.v_smallBox.v_value, smallBox.v_value);
assert.strictEqual(bigBox.v_smallBox.v_name, smallBox.v_name);
assert.notStrictEqual(bigBox.v_smallBox._address, smallBox._address);

smallBox._free();
bigBox._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();