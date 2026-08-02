# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun<T> foo(T x) ->T:
    return x

fun foo(StringLiteral str) ->StringLiteral:
    return str

#This is needed to generate the functions from the template
fun bar():
    foo(1)
    foo(0.25)

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that function templates work

assert.strictEqual($.foo(4), 4);
assert.strictEqual($.foo(0.25), 0.25);
assert.strictEqual($.foo("Hello"), "Hello");

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();