# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if the clone function works for numbers

const int = wrapper.IntWrapper.create(3);
const clonedInt = wrapper.IntWrapper.clone(int);
assert.strictEqual(int.value, clonedInt.value);
int._free();
clonedInt._free();

const other = wrapper.IntWrapper.create(4);
const other2 = wrapper.IntWrapper.create(8);
other.value = other2;
assert.strictEqual(other.value, 8);

other._free();
other2._free();

wrapper._detectMemoryLeaksDoNotUse();