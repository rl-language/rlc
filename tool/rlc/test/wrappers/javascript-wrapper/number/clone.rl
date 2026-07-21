# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the clone function works for numbers

const int = $.Int.create(3);
const clonedInt = $.Int.clone(int);
assert.strictEqual(int.value, clonedInt.value);
int._free();
clonedInt._free();

const other = $.Int.create(4);
const other2 = $.Int.create(8);
other.value = other2;
assert.strictEqual(other.value, 8);

other._free();
other2._free();

$._detectMemoryLeaksDoNotUse();