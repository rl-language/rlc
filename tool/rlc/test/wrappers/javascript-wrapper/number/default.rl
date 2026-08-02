# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the numbers get their default value

let int = $.Std.Int.create();
assert.strictEqual(int.value, 0);
int._free();

let float = $.Std.Float.create();
assert.strictEqual(float.value, 0);
float._free();

let bool = $.Std.Bool.create();
assert.strictEqual(bool.value, false);
bool._free();

let byte = $.Std.Byte.create();
assert.strictEqual(byte.value, 0);
byte._free();

$.Std._detectMemoryLeaksDoNotUse();