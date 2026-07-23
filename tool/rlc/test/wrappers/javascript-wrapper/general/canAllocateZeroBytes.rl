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
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that I can allocate zero bytes (i.e. I can have a class with no fields)




//Class
const empty = $.Cls_Empty.create();
assert.strictEqual(empty.f_getThree(), 3);
empty._free();



//Pointer
const ptr = $.Ptr_Cls_Empty.create($.Ptr_Cls_Empty.malloc());
assert.strictEqual(ptr.get().f_getThree(), 3);
$.free(ptr);
ptr._free();



//Array
const arr = $.Arr_2_Cls_Empty.create();
assert.strictEqual(arr.get(0).f_getThree(), 3);
assert.strictEqual(arr.get(1).f_getThree(), 3);
arr._free();



//Alternative
const alt = $.Alt2_Cls_Empty_Cls_OtherEmpty.create();
assert.strictEqual(alt.value.f_getThree(), 3);
alt._free();





$._detectMemoryLeaksDoNotUse();