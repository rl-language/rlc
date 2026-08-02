# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls SmallBox:
    Int value
    StringLiteral name

    fun init():
        self.value = 0
        self.name = "Unknown"

    fun multiplyValueBy2():
        self.value = self.value * 2

fun foo(Int x) ->Int:
    return x+1

fun foo(Float x) ->Float:
    return x*2.0

fun foo(Int x, Int y) ->Int:
    return x+y

fun foo(SmallBox smallBox) ->ref StringLiteral:
    return smallBox.name

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that function overloading works for top-level function

const int = $.Std.Int.create(5);
assert.strictEqual($.foo(int), 6);

const double = $.Std.Float.create(5);
assert.strictEqual($.foo(double), 10.0);
assert.strictEqual($.foo(4.5), 9.0);

const otherInt = $.Std.Int.create(10);
assert.strictEqual($.foo(int, otherInt), 15);
assert.strictEqual($.foo(int, 10), 15);
assert.strictEqual($.foo(5, otherInt), 15);
assert.strictEqual($.foo(5, 10), 15);

const smallBox = $.SmallBox.create();
const result = $.foo(smallBox);
assert.strictEqual(result.value, "Unknown");
result.value = "Hello";
assert.strictEqual(smallBox.name, "Hello");

int._free();
double._free();
otherInt._free();
smallBox._free();
$.Std.StringPool.free();

$.Std._detectMemoryLeaksDoNotUse();