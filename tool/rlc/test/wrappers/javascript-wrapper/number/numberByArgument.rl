# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(Int int, Float float, Bool bool, Byte byte):
    int = 25
    float = 0.003
    bool = true
    byte = byte(128)

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the numbers are correctly passed as function arguments and changed

let int;
let float;
let bool;
let byte;

function createObjects() {
    int = $.Int.create(7);
    float = $.Float.create(0.54);
    bool = $.Bool.create(false);
    byte = $.Byte.create(127);
}

function freeObjects() {
    int._free();
    float._free();
    bool._free();
    byte._free();
}

createObjects();
assert.strictEqual(int.value, 7);
assert.strictEqual(float.value, 0.54);
assert.strictEqual(bool.value, false);
assert.strictEqual(byte.value, 127);
freeObjects();

createObjects()
$.f_foo(int, float, bool, byte);
assert.strictEqual(int.value, 25);
assert.strictEqual(float.value, 0.003);
assert.strictEqual(bool.value, true);
assert.strictEqual(byte.value, -128);

assert.throws(() => foo(int, float));

byte.value = 0;
$.f_foo(3, 1.2, true, byte);
assert.strictEqual(byte.value, -128);

assert.throws(() => new Int(3));

freeObjects();

$._detectMemoryLeaksDoNotUse();