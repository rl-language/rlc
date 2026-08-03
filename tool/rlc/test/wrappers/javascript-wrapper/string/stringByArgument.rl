# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(StringLiteral stringLiteral):
    stringLiteral = "Modified inside Rulebook"

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if strings are correctly passed by function argument

let string;

string = $.Std.StringLiteral.create("Hello");
assert.strictEqual(string.value, "Hello");
string._free();

string = $.Std.StringLiteral.create("Hello");
$.foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string._free();

string = $.Std.StringLiteral.create("Hello");
$.foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string.value = "NewString";
assert.strictEqual(string.value, "NewString");
string._free();

string = $.Std.StringLiteral.create("Hello");
$.foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string.value = "NewString";
assert.strictEqual(string.value, "NewString");
$.foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string._free();

$.Std.StringPool.free();

$.Std._detectMemoryLeaksDoNotUse();