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

cls ClassWithArray:
    BigBox[2][3] arr

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if a class with an array as member field works

let bigBox = $.Cls_BigBox.create();
bigBox.v_smallBox.v_name = "Small";
let arr = $.Arr_2_3_Cls_BigBox.create([[bigBox, bigBox, bigBox], [bigBox, bigBox, bigBox]]);
bigBox.v_smallBox.v_name = "Big";
let myObj = $.Cls_ClassWithArray.create({ v_arr: arr});
for (let i = 0; i < myObj.v_arr.length[0]; i++) {
    for (let j = 0; j < myObj.v_arr.length[1]; j++) {
        assert.strictEqual(myObj.v_arr.get(i, j).v_smallBox.v_name, "Small");
    }
}

bigBox._free();
arr._free();
myObj._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();