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

fun foo1(Int[3][2][4] arr):
    1+1

fun foo2(BigBox[2][3] arr):
    let unused : BigBox[2][3]

fun foo3(StringLiteral[4] arr):
    1+1


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that creating an uninitialized array works

let arr;



//Array of StringLiteral
arr = $.strlit_4._createUninitialized();
arr.set("Hello", 0);
assert.strictEqual(arr.get(0), "Hello");

arr._free();





//Array of objects
arr = $.BigBox_2_3._createUninitialized();
arr.get(0, 0).value = 5;
assert.strictEqual(arr.get(0,0).value, 5);
    
let bigBox = $.BigBox.create();
arr.set(bigBox, [1,1]);
assert.strictEqual(arr.get(1,1).smallBox.name, "Unknown");

bigBox._free();
arr._free();





//Array of integers
arr = $.int64_t_3_2_4._createUninitialized();
arr.set(5, [0,0,0]);
assert.strictEqual(arr.get(0,0,0), 5);

arr._free();





$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();