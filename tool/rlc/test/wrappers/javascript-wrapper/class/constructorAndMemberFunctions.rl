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

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if constructors and member functions work



//Constructor
let smallBox = $.SmallBox.create({value: 123, name: "Gino"});
let bigBox = $.BigBox.create({value: 3});

assert.strictEqual(smallBox.value, 123);
assert.strictEqual(smallBox.name, "Gino");
assert.strictEqual(bigBox.value, 3);
assert.strictEqual(bigBox.smallBox.value, 0);
assert.strictEqual(bigBox.smallBox.name, "Unknown");

smallBox._free();
bigBox._free();

smallBox = $.SmallBox.create();
assert.strictEqual(smallBox.value, 0);
assert.strictEqual(smallBox.name, "Unknown");

smallBox._free();



//Member functions
let smallBox2 = $.SmallBox.create({value: 50});
smallBox2.multiplyValueBy2();
assert.strictEqual(smallBox2.value, 100);

smallBox2._free();




$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();