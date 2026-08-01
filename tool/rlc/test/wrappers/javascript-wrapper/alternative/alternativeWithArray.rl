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

fun foo(SmallBox | StringLiteral[4] alt):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that alternatives containing arrays work

let alt = $.Alt2_SmallBox_Arr_4_StringLiteral.create();
assert.strictEqual(alt.value.v_value, 0);
assert.strictEqual(alt.value.v_name, "Unknown");

let arr = $.Arr_4_StringLiteral.create();
arr.set("Hello", 1);
alt.value = arr;
assert.strictEqual(alt.value.get(0), "");
assert.strictEqual(alt.value.get(1), "Hello");
assert.strictEqual(alt.value.get(2), "");
assert.strictEqual(alt.value.get(3), "");

arr._free();
alt._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();