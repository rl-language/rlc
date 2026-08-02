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
const result = $.f_foo(obj);
assert.strictEqual(obj.v_alt.value.v_smallBox.v_name, "Super");
assert.strictEqual(result.value.v_smallBox.v_name, "Super");
assert.strictEqual(obj.v_alt._address, result._address);

const smallBox = $.SmallBox.create({ v_value: 64, v_name: "Nice" });
const bigBox = $.BigBox.create({ v_value: 10, v_smallBox: smallBox });
result.value = bigBox;
assert.strictEqual(obj.v_alt.value.v_value, 10);
assert.strictEqual(obj.v_alt.value.v_smallBox.v_value, 64);
assert.strictEqual(obj.v_alt.value.v_smallBox.v_name, "Nice");

smallBox._free();
bigBox._free();
obj._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();