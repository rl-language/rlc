# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

let int = wrapper.IntWrapper.create(3n);
assert.strictEqual(int.value, 3);
int.value = 123n;
assert.strictEqual(int.value, 123);
int.value = 100;
assert.strictEqual(int.value, 100);
int._free();

wrapper._detectMemoryLeaksDoNotUse();