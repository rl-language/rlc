# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if strings are correctly cloned

const original = wrapper.StringLiteralWrapper.create("Hello");
const cloned = wrapper.StringLiteralWrapper.clone(original);
assert.strictEqual(original.value, cloned.value);
original._free();
cloned._free();

const other = wrapper.StringLiteralWrapper.create("Hi");
const other2 = wrapper.StringLiteralWrapper.create("Hello");
other.value = other2;
assert.strictEqual(other.value, "Hello");
other._free();
other2._free();

wrapper.StringPool.free();

wrapper._detectMemoryLeaksDoNotUse();