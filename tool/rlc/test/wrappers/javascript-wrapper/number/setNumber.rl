# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if the value of the numbers is correctly changed

let num;

num = wrapper.IntWrapper.create(5);
assert.doesNotThrow(() => wrapper.IntWrapper._assertWrapper(num));
assert.strictEqual(num.value, 5);
num.value = -12345;
assert.strictEqual(num.value, -12345);
assert.throws(() => num.value = 1.23);
assert.throws(() => num.value = true);
assert.throws(() => num.value = "ABC");
num._free();

num = wrapper.FloatWrapper.create(-0.34);
assert.doesNotThrow(() => wrapper.FloatWrapper._assertWrapper(num));
assert.strictEqual(num.value, -0.34);
num.value = 3.57584;
assert.strictEqual(num.value, 3.57584);
assert.throws(() => num.value = true);
assert.throws(() => num.value = "ABC");
num._free();

num = wrapper.BoolWrapper.create(true);
assert.doesNotThrow(() => wrapper.BoolWrapper._assertWrapper(num));
assert.strictEqual(num.value, true);
num.value = false;
assert.strictEqual(num.value, false);
assert.throws(() => num.value = -0.23);
assert.throws(() => num.value = 77);
assert.throws(() => num.value = "ABC");
num._free();

num = wrapper.ByteWrapper.create(-129);
assert.doesNotThrow(() => wrapper.ByteWrapper._assertWrapper(num));
assert.strictEqual(num.value, 127);
num.value = 56;
assert.strictEqual(num.value, 56);
assert.throws(() => num.value = -0.23);
assert.throws(() => num.value = true);
assert.throws(() => num.value = "ABC");
num._free();

wrapper._detectMemoryLeaksDoNotUse();