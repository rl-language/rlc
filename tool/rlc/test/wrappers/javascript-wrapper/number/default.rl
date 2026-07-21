# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the numbers get their default value

let int = $.Int.create();
assert.strictEqual(int.value, 0);
int._free();

let float = $.Float.create();
assert.strictEqual(float.value, 0);
float._free();

let bool = $.Bool.create();
assert.strictEqual(bool.value, false);
bool._free();

let byte = $.Byte.create();
assert.strictEqual(byte.value, 0);
byte._free();

$._detectMemoryLeaksDoNotUse();