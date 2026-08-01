# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(StringLiteral s1, StringLiteral s2):
    s2 = s1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if strings are correctly passed as function arguments

let s1;
let s2;
    
s1 = $.StringLiteral.create("S1");
assert.strictEqual(s1.value, "S1");

s2 = $.StringLiteral.create("S2");
assert.strictEqual(s2.value, "S2");

$.f_foo(s1, s2);
assert.strictEqual(s1.value, "S1");
assert.strictEqual(s2.value, "S1");
assert.notStrictEqual(s1._address, s2._address);

s2.value = "ABC"
assert.strictEqual(s1.value, "S1");
assert.strictEqual(s2.value, "ABC");

s1._free();
s2._free();

$.StringPool.free();

$._detectMemoryLeaksDoNotUse();