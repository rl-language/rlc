# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if Int also works with BigInt (e.g. 3n)

let int = $.Int.create(3n);
assert.strictEqual(int.value, 3);

int.value = 123n;
assert.strictEqual(int.value, 123);

int.value = 100;
assert.strictEqual(int.value, 100);

int._free();

$._detectMemoryLeaksDoNotUse();