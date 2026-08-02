# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls BigBox:
    Int value
    Int _secretValue
    SmallBox smallBox

cls SmallBox:
    Int value
    StringLiteral name

    fun init():
        self.value = 0
        self.name = "Unknown"

    fun assign(SmallBox smallBox):
        self.name = "Assigned"

fun foo(Int | BigBox | StringLiteral alt):
    let x : Int | BigBox | StringLiteral
    alt = x


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let alt = $.alt_int64_t_or_BigBox_or_strlit.create();

alt.f_assign(5);
assert.strictEqual(alt.value, 5);

alt.f_assign("Hello");
assert.strictEqual(alt.value, "Hello");

let tmp = $.BigBox.create({v_value: 1024});
alt.f_assign(tmp);
assert.strictEqual(alt.value.v_value, 1024);

tmp._free();
alt._free();

let alt1 = $.alt_int64_t_or_BigBox_or_strlit.create();
let alt2 = $.alt_int64_t_or_BigBox_or_strlit.create();
alt2.value = "Hello";
alt1.f_assign(alt2);
assert.strictEqual(alt1.value, "Hello");

alt1._free();
alt2._free();
$.StringPool.free();
$._detectMemoryLeaksDoNotUse();