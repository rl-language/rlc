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

let bigBox = $.BigBox.create();
bigBox.smallBox.name = "Small";
let arr = $.BigBox_2_3.create([[bigBox, bigBox, bigBox], [bigBox, bigBox, bigBox]]);
bigBox.smallBox.name = "Big";
let myObj = $.ClassWithArray.create({ arr: arr});
for (let i = 0; i < myObj.arr.length[0]; i++) {
    for (let j = 0; j < myObj.arr.length[1]; j++) {
        assert.strictEqual(myObj.arr.get(i, j).smallBox.name, "Small");
    }
}

bigBox._free();
arr._free();
myObj._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();