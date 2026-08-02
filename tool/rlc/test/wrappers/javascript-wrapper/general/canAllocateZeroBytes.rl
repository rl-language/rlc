# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

cls Empty:
    pass

    fun getThree()->Int:
        return 3

cls OtherEmpty:
    pass

fun foo(OwningPtr<Empty> ptr, Empty[2] arr, Empty | OtherEmpty alt):
    let x : Empty[2]

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that I can allocate zero bytes (i.e. I can have a class with no fields)




//Class
const empty = $.Empty.create();
assert.strictEqual(empty.f_getThree(), 3);
empty._free();



//Pointer
const ptr = $.ptr_Empty.create($.ptr_Empty.calloc());
assert.strictEqual(ptr.get().f_getThree(), 3);
$.free(ptr);
ptr._free();



//Array
const arr = $.Empty_2.create();
assert.strictEqual(arr.get(0).f_getThree(), 3);
assert.strictEqual(arr.get(1).f_getThree(), 3);
arr._free();



//Alternative
const alt = $.alt_Empty_or_OtherEmpty.create();
assert.strictEqual(alt.value.f_getThree(), 3);
alt._free();





$._detectMemoryLeaksDoNotUse();