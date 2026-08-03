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

    fun multiplyValueBy2():
        self.value = self.value * 2

cls StrangeClass:
    SmallBox smallBox
    Int | BigBox | StringLiteral alt

fun foo(StrangeClass obj) -> ref Int | BigBox | StringLiteral:
    let bigBox : BigBox
    bigBox.smallBox.name = "Super"
    obj.alt = bigBox
    return obj.alt

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if alternatives are correctly returned by ref

const obj = $.StrangeClass.create();
const result = $.foo(obj);
assert.strictEqual(obj.alt.value.smallBox.name, "Super");
assert.strictEqual(result.value.smallBox.name, "Super");
assert.strictEqual(obj.alt._address, result._address);

const smallBox = $.SmallBox.create({ value: 64, name: "Nice" });
const bigBox = $.BigBox.create({ value: 10, smallBox: smallBox });
result.value = bigBox;
assert.strictEqual(obj.alt.value.value, 10);
assert.strictEqual(obj.alt.value.smallBox.value, 64);
assert.strictEqual(obj.alt.value.smallBox.name, "Nice");

smallBox._free();
bigBox._free();
obj._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();