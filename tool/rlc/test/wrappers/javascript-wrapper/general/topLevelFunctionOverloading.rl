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

const int = $.Int.create(5);
assert.strictEqual($.f_foo(int), 6);

const double = $.Float.create(5);
assert.strictEqual($.f_foo(double), 10.0);
assert.strictEqual($.f_foo(4.5), 9.0);

const otherInt = $.Int.create(10);
assert.strictEqual($.f_foo(int, otherInt), 15);
assert.strictEqual($.f_foo(int, 10), 15);
assert.strictEqual($.f_foo(5, otherInt), 15);
assert.strictEqual($.f_foo(5, 10), 15);

const smallBox = $.Cls_SmallBox.create();
const result = $.f_foo(smallBox);
assert.strictEqual(result.value, "Unknown");
result.value = "Hello";
assert.strictEqual(smallBox.v_name, "Hello");

int._free();
double._free();
otherInt._free();
smallBox._free();
$.StringPool.free();

$._detectMemoryLeaksDoNotUse();