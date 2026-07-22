# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Check that there are no leaks if instantiation fails

assert.throws(() => $.Int.create("Hello"));
$._detectMemoryLeaksDoNotUse();