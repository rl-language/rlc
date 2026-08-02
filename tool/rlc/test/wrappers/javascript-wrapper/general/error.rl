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
assert.throws(() => $.SmallBox.create({v_name: 5}));
assert.throws(() => $.alt_int64_t_or_BigBox_or_strlit.create(0.5));
assert.throws(() => $.strlit_4.create(["Hello"]));
assert.throws(() => $.ColorEnum.create("Hello"));
assert.throws(() => $.ptr_Int.create("Hello"));

    
let obj;

obj = $.Int.create();
assert.throws(() => $.StringLiteral._assertWrapper(obj));
assert.throws(() => obj.value = "Hello");
obj._free();

obj = $.StringLiteral.create();
assert.throws(() => $.Int._assertWrapper(obj));
assert.throws(() => obj.value = 5);
obj._free();

obj = $.BigBox.create();
assert.throws(() => $.SmallBox._assertWrapper(obj));
assert.throws(() => obj.v_smallBox = obj);
obj._free();

obj = $.alt_int64_t_or_BigBox_or_strlit.create();
assert.throws(() => $.strlit_4._assertWrapper(obj));
assert.throws(() => obj.value = obj);
obj._free();

obj = $.strlit_4.create();
assert.throws(() => $.alt_int64_t_or_BigBox_or_strlit._assertWrapper(obj));
assert.throws(() => obj.set(0, obj));
obj._free();

obj = $.ColorEnum.create();
assert.throws(() => $.BigBox._assertWrapper(obj));
assert.throws(() => obj.value = obj);
obj._free();

obj = $.ptr_Int.create();
assert.throws(() => $.Int._assertWrapper(obj));
assert.throws(() => obj.set("Hello"));
obj._free();


obj = $.BigBox.create();
assert.throws(() => obj.doesNotExist = 3);
obj._free();


assert.throws(() => $.BigBox.create({doesNotExist: 3}));
assert.throws(() => new $.StringPool());
assert.throws(() => new $.Int());



$.StringPool.free();
$._detectMemoryLeaksDoNotUse();