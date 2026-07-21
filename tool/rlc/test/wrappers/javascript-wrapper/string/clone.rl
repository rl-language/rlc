# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if strings are correctly cloned

const original = $.StringLiteral.create("Hello");
const cloned = $.StringLiteral.clone(original);
assert.strictEqual(original.value, cloned.value);
original._free();
cloned._free();

const other = $.StringLiteral.create("Hi");
const other2 = $.StringLiteral.create("Hello");
other.value = other2;
assert.strictEqual(other.value, "Hello");
other._free();
other2._free();

$.StringPool.free();

$._detectMemoryLeaksDoNotUse();