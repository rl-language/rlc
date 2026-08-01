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

fun foo(ClassWithArray obj) ->ref BigBox[2][3]:
    return obj.arr

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if arrays are correctly returned by ref

const smallBox = $.SmallBox.create();
smallBox.v_name = "Hello";
const bigBox = $.BigBox.create({v_smallBox: smallBox});
const obj = $.Cls_ClassWithArray.create();
const result = $.f_foo(obj);
result.set(bigBox, [1, 1]);
for (let i = 0; i < result.length[0]; i++) {
    for (let j = 0; j < result.length[1]; j++) {
        let str;
        if(i === 1 && j === 1){
            str = "Hello";
        }
        else{
            str = "Unknown";
        }

        assert.strictEqual(obj.v_arr.get(i, j).v_smallBox.v_name, str);
    }
}

smallBox._free();
bigBox._free();
obj._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();