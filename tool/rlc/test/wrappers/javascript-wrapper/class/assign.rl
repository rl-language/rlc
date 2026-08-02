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

    fun assign(SmallBox smallBox):
        self.name = "Assigned"


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let bigBox1 = $.BigBox.create();
let bigBox2 = $.BigBox.create({v_value: 256});

bigBox1.f_assign(bigBox2);
assert.strictEqual(bigBox1.v_value, 256);
assert.strictEqual(bigBox1.v_smallBox.v_name, "Assigned");

bigBox1._free();
bigBox2._free();
$._detectMemoryLeaksDoNotUse();