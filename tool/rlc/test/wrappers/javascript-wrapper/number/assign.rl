# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let int1 = $.Std.Int.create(64);
let int2 = $.Std.Int.create(256);

int1.assign(int2);
assert.strictEqual(int1.value, 256);

int2.assign(1024);
assert.strictEqual(int2.value, 1024);

int1._free();
int2._free();
$.Std._detectMemoryLeaksDoNotUse();