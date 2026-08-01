# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo(StringLiteral str) ->ref StringLiteral:
    str = "Changed from Rulebook"
    return str

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if strings are correctly returned by ref

let input = $.StringLiteral.create("From Javascript");
const ref = $.f_foo(input);
assert.strictEqual(input._address, ref._address);
assert.strictEqual(input.value, "Changed from Rulebook");
assert.strictEqual(ref.value, "Changed from Rulebook");

ref.value = "Another value";
assert.strictEqual(ref.value, "Another value");
assert.strictEqual(input.value, "Another value");

input._free();

$.StringPool.free();

$._detectMemoryLeaksDoNotUse();