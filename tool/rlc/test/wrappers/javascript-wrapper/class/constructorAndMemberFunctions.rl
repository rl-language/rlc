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
let smallBox = $.SmallBox.create({v_value: 123, v_name: "Gino"});
let bigBox = $.BigBox.create({v_value: 3});

assert.strictEqual(smallBox.v_value, 123);
assert.strictEqual(smallBox.v_name, "Gino");
assert.strictEqual(bigBox.v_value, 3);
assert.strictEqual(bigBox.v_smallBox.v_value, 0);
assert.strictEqual(bigBox.v_smallBox.v_name, "Unknown");

smallBox._free();
bigBox._free();

smallBox = $.SmallBox.create();
assert.strictEqual(smallBox.v_value, 0);
assert.strictEqual(smallBox.v_name, "Unknown");

smallBox._free();



//Member functions
let smallBox2 = $.SmallBox.create({v_value: 50});
smallBox2.f_multiplyValueBy2();
assert.strictEqual(smallBox2.v_value, 100);

smallBox2._free();




$.StringPool.free();
$._detectMemoryLeaksDoNotUse();