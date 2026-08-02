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

const objInt = $.MyClassTint64_tT.create({value: 7});
assert.strictEqual(objInt.value, 7);
objInt._free();

const objSmallBox = $.MyClassTSmallBoxT.create();
assert.strictEqual(objSmallBox.value.name, "Unknown");
objSmallBox._free();

const objMultipleTemplate = $.MyClassTOtherClassTint64_t_2TT.create();
assert.strictEqual(objMultipleTemplate.value.otherValue.get(0), 0);
assert.strictEqual(objMultipleTemplate.value.otherValue.get(1), 0);
objMultipleTemplate._free();

const objDouble = $.DoubleTemplateTstrlitTdoubleT.create();
objDouble.first = "Hello";
objDouble.second = 0.25;
assert.strictEqual(objDouble.first, "Hello");
assert.strictEqual(objDouble.second, 0.25);
objDouble._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();