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
    let x : Int[3][2][4]

fun foo2(BigBox[2][3] arr):
    let unused : BigBox[2][3]

fun foo3(StringLiteral[4] arr):
    let x : StringLiteral[4]

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if we can write into arrays

let arrData;
let arr;





//Array of StringLiterals

function checkData1() {
    for (let i = 0; i < arr.length; i++) {
        assert.strictEqual(arr.get(i), arrData[i]);
    }
}

arrData = ["Str1", "Str2", "Str3", "Str4"];
arr = $.strlit_4.create(arrData);
checkData1();

arr.set("Hello", 1);
arrData[1] = "Hello";
checkData1();

assert.throws(() => arr.get(arrData.length));
assert.throws(() => arr.get(-1));
assert.throws(() => arr.set(0.43, 0));

arr._free();





//Array of objects

function checkData2() {
    for (let i = 0; i < arr.length[0]; i++) {
        for (let j = 0; j < arr.length[1]; j++) {
            let fromArr = arr.get(i, j);
            let fromArrData = arrData[i][j];

            assert.strictEqual(fromArr.value, fromArrData.value);
            assert.strictEqual(fromArr.smallBox.value, fromArrData.smallBox.value);
            assert.strictEqual(fromArr.smallBox.name, fromArrData.smallBox.name);
        }
    }
}

let bigBox = $.BigBox.create();
arrData = [[bigBox, bigBox, bigBox], [bigBox, bigBox, bigBox]];
arr = $.BigBox_2_3.create(arrData);
checkData2();

let otherBigBox = $.BigBox.create({ value: 123 });
arr.set(otherBigBox, [1, 1]);
arrData[1][1] = otherBigBox;
checkData2();

assert.throws(() => arr.get(9, 0));
assert.throws(() => arr.get(-1, 3));
assert.throws(() => arr.set("Hello", [0,0]));

bigBox._free();
otherBigBox._free();
arr._free();





//Array of integers

function checkData3() {
    for (let i = 0; i < arr.length[0]; i++) {
        for (let j = 0; j < arr.length[1]; j++) {
            for (let k = 0; k < arr.length[2]; k++) {
                assert.strictEqual(arr.get(i, j, k), arrData[i][j][k]);
            }
        }
    }
}

arrData = [
    [
        [0, 1, 2, 3],
        [4, 5, 6, 7]
    ],
    [
        [8, 9, 10, 11],
        [12, 13, 14, 15]
    ],
    [
        [16, 17, 18, 19],
        [20, 21, 22, 23]
    ]
];

arr = $.int64_t_3_2_4.create(arrData);
checkData3();

arr.set(7, [1, 1, 2]);
arrData[1][1][2] = 7;
checkData3();

assert.throws(() => arr.get(-1, 0, 0));
assert.throws(() => arr.get(0, 7, 0));
assert.throws(() => arr.set("Hello", [0,0,0]));

arr._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();