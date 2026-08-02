# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(Int | Float | Byte alt):
    let x : Int | Float | Byte
    alt = 3

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that an alternative Int | Float | Byte works

let alt = $.alt_int64_t_or_double_or_int8_t.create(4);
assert.strictEqual(alt.value, 4);

alt.value = 2.3;
assert.strictEqual(alt.value, 2.3);

alt._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();