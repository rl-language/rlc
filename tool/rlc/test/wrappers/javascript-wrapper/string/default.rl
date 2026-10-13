# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if strings correctly take the default value

let str = $.Std.StringLiteral.create();
assert.strictEqual(str.value, "");

str._free();
$.Std.StringPool.free();

$.Std._detectMemoryLeaksDoNotUse();