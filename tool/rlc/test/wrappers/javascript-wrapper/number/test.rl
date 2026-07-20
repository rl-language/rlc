# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo() -> Int:
    return 42

#--- test.mjs
import assert from 'assert';
import * as wrapper from './wrapper.mjs';

assert.strictEqual(wrapper.fun$foo(), 42);
console.log("SUCCESS");