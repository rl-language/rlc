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

//Checking if arrays are initialized correctly with their default values

function run(arr, checkData){
    checkData(arr);
    arr._free();
}

function checkData1(arr) {
    for (let i = 0; i < arr.length; i++) {
        assert.strictEqual(arr.get(i), "");
    }
}

function checkData2(arr) {
    for (let i = 0; i < arr.length[0]; i++) {
        for (let j = 0; j < arr.length[1]; j++) {
            const myObj = arr.get(i, j);
            assert.strictEqual(myObj.v_value, 0);
            assert.strictEqual(myObj.v_smallBox.v_value, 0);
            assert.strictEqual(myObj.v_smallBox.v_name, "Unknown");
        }
    }
}

function checkData3(arr) {
    for (let i = 0; i < arr.length[0]; i++) {
        for (let j = 0; j < arr.length[1]; j++) {
            for(let k = 0; k < arr.length[2]; k++){
                assert.strictEqual(arr.get(i, j, k), 0);
            }
        }
    }
}


let arr;

arr = $.Arr_4_StringLiteral.create();
run(arr, checkData1);

arr = $.Arr_2_3_Cls_BigBox.create();
run(arr, checkData2);

arr = $.Arr_3_2_4_Int.create();
run(arr, checkData3);

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();