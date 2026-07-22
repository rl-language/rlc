# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls Bad:
    Int value
    fun init(){false}:
        1+1

fun foo(Bad[3] arr, Bad | Byte[99] alt, OwningPtr<Bad> ptr):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Check that there are no leaks if instantiation fails

assert.throws(() => $.Int.create("Hello"));
assert.throws(() => $.Cls_Bad.create());
assert.throws(() => $.Arr_3_Cls_Bad.create());
assert.throws(() => $.Alt2_Cls_Bad_Arr_99_Byte.create());
//assert.throws(() => $.Ptr_Cls_Bad.malloc(5));

$._detectMemoryLeaksDoNotUse();