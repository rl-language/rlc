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
    1+1

fun foo3(StringLiteral[4] arr):
    1+1


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that creating an uninitialized array works

let arr;



//Array of StringLiteral
arr = $.Arr_4_StringLiteral._createUninitialized();
arr.set("Hello", 0);
assert.strictEqual(arr.get(0), "Hello");

arr._free();





//Array of objects
arr = $.Arr_2_3_Cls_BigBox._createUninitialized();
arr.get(0, 0).v_value = 5;
assert.strictEqual(arr.get(0,0).v_value, 5);
    
let bigBox = $.Cls_BigBox.create();
arr.set(bigBox, [1,1]);
assert.strictEqual(arr.get(1,1).v_smallBox.v_name, "Unknown");

bigBox._free();
arr._free();





//Array of integers
arr = $.Arr_3_2_4_Int._createUninitialized();
arr.set(5, [0,0,0]);
assert.strictEqual(arr.get(0,0,0), 5);

arr._free();





$.StringPool.free();
$._detectMemoryLeaksDoNotUse();