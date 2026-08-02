# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let str1 = $.StringLiteral.create("Hello");
let str2 = $.StringLiteral.create("Hi");

str1.f_assign(str2);
assert.strictEqual(str1.value, "Hi");

str2.f_assign("Hey");
assert.strictEqual(str2.value, "Hey");

str1._free();
str2._free();
$.StringPool.free();
$._detectMemoryLeaksDoNotUse();