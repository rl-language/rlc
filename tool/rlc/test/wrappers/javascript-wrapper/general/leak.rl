# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls Bad:
    Int value
    fun init(){false}:
        1+1

cls Container:
    Int value

fun abc(Bad[3] arr, Bad | Byte[99] alt, OwningPtr<Bad> ptr):
    1+1

fun foo1(Int x, Int y, Container z)->Container{false}:
    let container : Container
    return container

fun foo2(Int x, Int y) ->Int{false}:
    return x + y

fun foo3(Container c) -> ref Container{false}:
    return c

fun foo4(Int x) -> ref Int{false}:
    return x

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Check that there are no leaks if instantiation fails

assert.throws(() => $.Int.create("Hello"));
assert.throws(() => $.Cls_Bad.create());
assert.throws(() => $.Arr_3_Cls_Bad.create());
assert.throws(() => $.Alt2_Cls_Bad_Arr_99_Byte.create());


//Check that there are no leaks if functions fail

let container;
container = $.Cls_Container.create();
assert.throws(() => $.f_foo1(1,2,container));
container._free();

assert.throws(() => $.f_foo2(1,2));

container = $.Cls_Container.create();
assert.throws(() => $.f_foo3(container));
container._free();

assert.throws(() => $.f_foo4(5));

//assert.throws(() => $.Ptr_Cls_Bad.malloc(5));

$._detectMemoryLeaksDoNotUse();