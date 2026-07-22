# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo() ->StringLiteral[4]:
    let arr : StringLiteral[4]
    arr[0] = "W"
    arr[1] = "X"
    arr[2] = "Y"
    arr[3] = "Z"
    return arr

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if arrays are correctly returned by functions

const result = $.f_foo();
assert.strictEqual(result.get(0), "W");
assert.strictEqual(result.get(1), "X");
assert.strictEqual(result.get(2), "Y");
assert.strictEqual(result.get(3), "Z");

result._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();