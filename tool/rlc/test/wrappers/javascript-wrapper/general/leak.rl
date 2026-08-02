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
assert.throws(() => $.Bad.create());
assert.throws(() => $.Bad_3.create());
assert.throws(() => $.alt_Bad_int8_t_99.create());


//Check that there are no leaks if functions fail

let container;
container = $.Container.create();
assert.throws(() => $.foo1(1,2,container));
container._free();

assert.throws(() => $.foo2(1,2));

container = $.Container.create();
assert.throws(() => $.foo3(container));
container._free();

assert.throws(() => $.foo4(5));

assert.throws(() => $.ptr_Bad.calloc(5));

$._detectMemoryLeaksDoNotUse();