# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let str1 = $.Std.StringLiteral.create("Hello");
let str2 = $.Std.StringLiteral.create("Hi");

str1.assign(str2);
assert.strictEqual(str1.value, "Hi");

str2.assign("Hey");
assert.strictEqual(str2.value, "Hey");

str1._free();
str2._free();
$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();