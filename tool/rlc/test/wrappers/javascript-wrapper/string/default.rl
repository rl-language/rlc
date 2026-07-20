# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

//Checking if strings correctly take the default value

let str = wrapper.StringLiteralWrapper.create();
assert.strictEqual(str.value, "");

str._free();
wrapper.StringPool.free();

wrapper._detectMemoryLeaksDoNotUse();