# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

using Arr = Int[3]
#This is simply Int[3][2]
fun foo(Arr[2] arr):
    let x : Arr[2]

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if arrays of arrays work

const arr = $.int64_t_3_2.create();
arr.set(64, [0,0]);
arr.set(64, [1,0]);
arr.set(64, [2,0]);

for(let i=0; i<3; i++){
    for(let j=0; j<2; j++){
        assert.strictEqual(arr.get(i, j), j == 0 ? 64 : 0);
    }
}

arr._free();

$.Std._detectMemoryLeaksDoNotUse();