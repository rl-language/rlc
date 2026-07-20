# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(StringLiteral stringLiteral):
    stringLiteral = "Modified inside Rulebook"

#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if strings are correctly passed by function argument

let string;

string = wrapper.StringLiteralWrapper.create("Hello");
assert.strictEqual(string.value, "Hello");
string._free();

string = wrapper.StringLiteralWrapper.create("Hello");
wrapper.fun$foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string._free();

string = wrapper.StringLiteralWrapper.create("Hello");
wrapper.fun$foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string.value = "NewString";
assert.strictEqual(string.value, "NewString");
string._free();

string = wrapper.StringLiteralWrapper.create("Hello");
wrapper.fun$foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string.value = "NewString";
assert.strictEqual(string.value, "NewString");
wrapper.fun$foo(string);
assert.strictEqual(string.value, "Modified inside Rulebook");
string._free();

wrapper.StringPool.free();

wrapper._detectMemoryLeaksDoNotUse();