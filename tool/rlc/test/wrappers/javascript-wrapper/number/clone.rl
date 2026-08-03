# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the clone function works for numbers

const int = $.Std.Int.create(3);
const clonedInt = $.Std.Int.clone(int);
assert.strictEqual(int.value, clonedInt.value);
int._free();
clonedInt._free();

const other = $.Std.Int.create(4);
const other2 = $.Std.Int.create(8);
other.value = other2;
assert.strictEqual(other.value, 8);

other._free();
other2._free();

$.Std._detectMemoryLeaksDoNotUse();