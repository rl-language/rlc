# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(Int | Float | Byte alt):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that an alternative Int | Float | Byte works

let alt = $.Alt3_Int_Float_Byte.create(4);
assert.strictEqual(alt.value, 4);

alt.value = 2.3;
assert.strictEqual(alt.value, 2.3);

alt._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();