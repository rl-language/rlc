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

let smallBox = $.SmallBox.create({value: 50, name: "Gino"});
let bigBox = $.BigBox.create();
bigBox.smallBox = smallBox;
assert.strictEqual(bigBox.smallBox.value, smallBox.value);
assert.strictEqual(bigBox.smallBox.name, smallBox.name);
assert.notStrictEqual(bigBox.smallBox._address, smallBox._address);

smallBox._free();
bigBox._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();