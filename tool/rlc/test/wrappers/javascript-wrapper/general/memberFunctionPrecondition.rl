# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

cls MyClass:
    fun positiveSum(Int x, Int y)->Int{x>0 and y>0}:
        return x + y


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if preconditions work and if I can call the "can" functions for member functions

const obj = $.MyClass.create();

assert.strictEqual(obj.positiveSum(1,2), 3);
assert.throws(() => obj.positiveSum(0,2));

assert.strictEqual(obj.can_positiveSum(1,2), true);
assert.strictEqual(obj.can_positiveSum(0,2), false);

obj._free();
$.Std._detectMemoryLeaksDoNotUse();