# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
#I need this function to generate Arr_4_StringLiteral
fun foo(StringLiteral[4] arr):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if arrays are cloned correctly

const original = $.Arr_4_StringLiteral.create(["A", "B", "C", "D"]);
const cloned = $.Arr_4_StringLiteral.clone(original);
for(let i=0; i<original.length; i++){
    assert.strictEqual(original.get(i).value, cloned.get(i).value);
}
    
original._free();
cloned._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();