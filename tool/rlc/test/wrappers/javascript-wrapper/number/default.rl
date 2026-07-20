# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if the numbers get their default value

let int = wrapper.IntWrapper.create();
assert.strictEqual(int.value, 0);
int._free();

let float = wrapper.FloatWrapper.create();
assert.strictEqual(float.value, 0);
float._free();

let bool = wrapper.BoolWrapper.create();
assert.strictEqual(bool.value, false);
bool._free();

let byte = wrapper.ByteWrapper.create();
assert.strictEqual(byte.value, 0);
byte._free();

wrapper._detectMemoryLeaksDoNotUse();