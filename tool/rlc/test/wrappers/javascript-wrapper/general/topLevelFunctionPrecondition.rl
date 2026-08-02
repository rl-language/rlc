# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

fun positiveSum(Int x, Int y)->Int{x>0 and y>0}:
    return x + y

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if preconditions work and if I can call the "can" functions

assert.strictEqual($.positiveSum(1,2), 3);
assert.throws(() => $.positiveSum(0,2));

assert.strictEqual($.can_positiveSum(1,2), true);
assert.strictEqual($.can_positiveSum(0,2), false);

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();