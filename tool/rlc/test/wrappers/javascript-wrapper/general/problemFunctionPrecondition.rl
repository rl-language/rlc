# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

fun can_foo() ->Bool:
    return false

fun foo(){true}:
    1+1




fun bar(){true}:
    1+1

fun can_bar() ->Bool:
    return false



cls MyClass:
    fun can_foo() ->Bool:
        return false

    fun foo(){true}:
        1+1




    fun bar(){true}:
        1+1

    fun can_bar() ->Bool:
        return false

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

/*
If I declare a function with the same signature of the function precondition
and that function is placed before the original, then that function is wrongly
considered as the precondition.
*/

assert.strictEqual($.f_can_foo(), false);
assert.strictEqual($.f_can_bar(), true);

let obj = $.MyClass.create();
assert.strictEqual(obj.f_can_foo(), false);
assert.strictEqual(obj.f_can_bar(), true);

obj._free();
$._detectMemoryLeaksDoNotUse();