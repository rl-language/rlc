# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls<T> MyClass:
    T value

cls<T> OtherClass:
    T otherValue

cls<T, U> DoubleTemplate:
    T first
    U second

cls SmallBox:
    Int value
    StringLiteral name

    fun init():
        self.value = 0
        self.name = "Unknown"

    fun multiplyValueBy2():
        self.value = self.value * 2

#This is needed to generate the classes from the template
fun foo(MyClass<Int> arg1, MyClass<SmallBox> arg2, MyClass<OtherClass<Int[2]>> arg3, DoubleTemplate<StringLiteral, Float> arg4):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if templates work

const objInt = $.MyClassTint64_tT.create({v_value: 7});
assert.strictEqual(objInt.v_value, 7);
objInt._free();

const objSmallBox = $.MyClassTSmallBoxT.create();
assert.strictEqual(objSmallBox.v_value.v_name, "Unknown");
objSmallBox._free();

const objMultipleTemplate = $.MyClassTOtherClassTint64_t_2TT.create();
assert.strictEqual(objMultipleTemplate.v_value.v_otherValue.get(0), 0);
assert.strictEqual(objMultipleTemplate.v_value.v_otherValue.get(1), 0);
objMultipleTemplate._free();

const objDouble = $.DoubleTemplateTstrlitTdoubleT.create();
objDouble.v_first = "Hello";
objDouble.v_second = 0.25;
assert.strictEqual(objDouble.v_first, "Hello");
assert.strictEqual(objDouble.v_second, 0.25);
objDouble._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();