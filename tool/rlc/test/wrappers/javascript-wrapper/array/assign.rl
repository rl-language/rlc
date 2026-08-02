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

fun foo(BigBox[2] arr1, Int[2] arr2):
    let x : BigBox[2]
    arr1 = x

    let y : Int[2]
    arr2 = y


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let arr1;
let arr2;



//BigBox[2]
arr1 = $.BigBox_2.create();
arr2 = $.BigBox_2.create();

arr1.f_assign(arr2);

assert.strictEqual(arr1.get(0).v_smallBox.v_name, "Assigned");
assert.strictEqual(arr1.get(1).v_smallBox.v_name, "Assigned");

arr1._free();
arr2._free();




//Int[2]
arr1 = $.int64_t_2.create();
arr2 = $.int64_t_2.create();

arr2.set(10, 0);
let tmp = $.Int.create(20);
arr2.set(tmp, 1);

arr1.f_assign(arr2);

assert.strictEqual(arr1.get(0), 10);
assert.strictEqual(arr1.get(1), 20);

arr1._free();
arr2._free();
tmp._free();




$._detectMemoryLeaksDoNotUse();