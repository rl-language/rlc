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

const smallBox = $.Cls_SmallBox.create({v_value: 10, v_name: "Small"});
const original = $.Cls_BigBox.create({v_value: 248, v_smallBox: smallBox});
const cloned = $.Cls_BigBox.clone(original);
assert.strictEqual(original.v_value, cloned.v_value);
assert.strictEqual(original.v_smallBox.v_value, cloned.v_smallBox.v_value);
assert.strictEqual(original.v_smallBox.v_name, cloned.v_smallBox.v_name);

smallBox._free();
original._free();
cloned._free();
$.StringPool.free();

$._detectMemoryLeaksDoNotUse();