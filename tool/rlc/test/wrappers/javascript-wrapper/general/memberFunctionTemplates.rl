# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl

cls MyClass:
    fun<T> foo(T x) ->T:
        return x

    fun foo(StringLiteral str) ->StringLiteral:
        return str

#This is needed to generate the functions from the template
fun bar():
    let obj : MyClass
    obj.foo(1)
    obj.foo(0.25)

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that member function templates work

const obj = $.MyClass.create();
assert.strictEqual(obj.foo(3), 3);
assert.strictEqual(obj.foo(0.25), 0.25);
assert.strictEqual(obj.foo("Hello"), "Hello");

obj._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();