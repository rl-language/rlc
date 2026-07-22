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

enum ColorEnum:
    red
    green
    blue

    fun getName()->StringLiteral:
        if self.value == ColorEnum::red.value:
            return "Red"
        if self.value == ColorEnum::green.value:
            return "Green"
        if self.value == ColorEnum::blue.value:
            return "Blue"

        return "Unknown color"

#I need this function to generate the alternative and the array
fun foo(Int | BigBox | StringLiteral alt, StringLiteral[4] arr, OwningPtr<Int> ptr):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if exceptions are properly thrown

assert.throws(() => $.Int.create("Hello"));
assert.throws(() => $.StringLiteral.create(4));
assert.throws(() => $.Cls_SmallBox.create({v_name: 5}));
assert.throws(() => $.Alt3_Int_Cls_BigBox_StringLiteral.create(0.5));
assert.throws(() => $.Arr_StringLiteral_4.create(["Hello"]));
assert.throws(() => $.Enum_ColorEnum.create("Hello"));
assert.throws(() => $.Ptr_Int.create("Hello"));

    
let obj;

obj = $.Int.create();
assert.throws(() => $.StringLiteral._assertWrapper(obj));
assert.throws(() => obj.value = "Hello");
obj._free();

obj = $.StringLiteral.create();
assert.throws(() => $.Int._assertWrapper(obj));
assert.throws(() => obj.value = 5);
obj._free();

obj = $.Cls_BigBox.create();
assert.throws(() => $.Cls_SmallBox._assertWrapper(obj));
assert.throws(() => obj.v_smallBox = obj);
obj._free();

obj = $.Alt3_Int_Cls_BigBox_StringLiteral.create();
assert.throws(() => $.Arr_StringLiteral_4._assertWrapper(obj));
assert.throws(() => obj.value = obj);
obj._free();

obj = $.Arr_StringLiteral_4.create();
assert.throws(() => $.Alt3_Int_Cls_BigBox_StringLiteral._assertWrapper(obj));
assert.throws(() => obj.set(0, obj));
obj._free();

obj = $.Enum_ColorEnum.create();
assert.throws(() => $.Cls_BigBox._assertWrapper(obj));
assert.throws(() => obj.value = obj);
obj._free();

obj = $.Ptr_Int.create();
assert.throws(() => $.Int._assertWrapper(obj));
assert.throws(() => obj.set("Hello"));
obj._free();


obj = $.Cls_BigBox.create();
assert.throws(() => obj.doesNotExist = 3);
obj._free();


assert.throws(() => $.Cls_BigBox.create({doesNotExist: 3}));
assert.throws(() => new $.StringPool());
assert.throws(() => new $.Int());



$.StringPool.free();
$._detectMemoryLeaksDoNotUse();