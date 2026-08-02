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

using Alt = Int | BigBox | StringLiteral
fun foo(Alt[2] alt):
    let x : Alt[2] #Needed to generate rl_m_init__Alt_2

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if arrays of alternatives work

let arr = $.Alt_2.create();
assert.strictEqual(arr.get(0).value, 0);
assert.strictEqual(arr.get(1).value, 0);

let alt = $.Alt.create(3);
arr.set(alt, 0);
assert.strictEqual(arr.get(0).value, 3);

alt._free();
arr._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();