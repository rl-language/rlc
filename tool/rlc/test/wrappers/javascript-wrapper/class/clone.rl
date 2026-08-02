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

//Checking if cloning an object works

const smallBox = $.SmallBox.create({value: 10, name: "Small"});
const original = $.BigBox.create({value: 248, smallBox: smallBox});
const cloned = $.BigBox.clone(original);
assert.strictEqual(original.value, cloned.value);
assert.strictEqual(original.smallBox.value, cloned.smallBox.value);
assert.strictEqual(original.smallBox.name, cloned.smallBox.name);

smallBox._free();
original._free();
cloned._free();
$.StringPool.free();

$._detectMemoryLeaksDoNotUse();