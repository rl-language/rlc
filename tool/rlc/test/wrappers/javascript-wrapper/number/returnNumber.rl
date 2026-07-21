# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo1() ->Int:
    return 5

fun foo2() ->Float:
    return 0.97

fun foo3() ->Bool:
    return true

fun foo4() ->Byte:
    return byte(128)

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if numbers are correctly returned by value
assert.strictEqual($.f_foo1(), 5);
assert.strictEqual($.f_foo2(), 0.97);
assert.strictEqual($.f_foo3(), true);
assert.strictEqual($.f_foo4(), -128);

$._detectMemoryLeaksDoNotUse();