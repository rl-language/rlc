# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo() ->StringLiteral:
    return "String from Rulebook"

#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if strings are correctly returned by functions

const result = wrapper.fun$foo();
assert.strictEqual(result, "String from Rulebook");

wrapper.StringPool.free();

wrapper._detectMemoryLeaksDoNotUse();