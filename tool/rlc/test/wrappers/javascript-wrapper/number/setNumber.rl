# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the value of the numbers is correctly changed

let num;

num = $.Std.Int.create(5);
assert.doesNotThrow(() => $.Std.Int._assertWrapper(num));
assert.strictEqual(num.value, 5);
num.value = -12345;
assert.strictEqual(num.value, -12345);
assert.throws(() => num.value = 1.23);
assert.throws(() => num.value = true);
assert.throws(() => num.value = "ABC");
num._free();

num = $.Std.Float.create(-0.34);
assert.doesNotThrow(() => $.Std.Float._assertWrapper(num));
assert.strictEqual(num.value, -0.34);
num.value = 3.57584;
assert.strictEqual(num.value, 3.57584);
assert.throws(() => num.value = true);
assert.throws(() => num.value = "ABC");
num._free();

num = $.Std.Bool.create(true);
assert.doesNotThrow(() => $.Std.Bool._assertWrapper(num));
assert.strictEqual(num.value, true);
num.value = false;
assert.strictEqual(num.value, false);
assert.throws(() => num.value = -0.23);
assert.throws(() => num.value = 77);
assert.throws(() => num.value = "ABC");
num._free();

num = $.Std.Byte.create(-129);
assert.doesNotThrow(() => $.Std.Byte._assertWrapper(num));
assert.strictEqual(num.value, 127);
num.value = 56;
assert.strictEqual(num.value, 56);
assert.throws(() => num.value = -0.23);
assert.throws(() => num.value = true);
assert.throws(() => num.value = "ABC");
num._free();

$.Std._detectMemoryLeaksDoNotUse();