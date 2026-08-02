# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

fun foo(OwningPtr<Int> ptr):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the assign function works

let ptr1 = $.ptr_Int.create($.ptr_Int.calloc());
let ptr2 = $.ptr_Int.create($.ptr_Int.calloc());

$.free(ptr1);
ptr1.assign(ptr2);
assert.strictEqual(ptr1.value, ptr2.value);
$.free(ptr1);

ptr1._free();
ptr2._free();
$._detectMemoryLeaksDoNotUse();