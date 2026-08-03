# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls MyClass:
    StringLiteral name

    fun name() ->StringLiteral:
        return "Hello"

enum MyEnum:
    val1
    val2
    
    fun value() ->Int:
        return 100

cls MyOtherClass:
    Int n

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the wrapper properly handles name collisions

const myClass = $.MyClass.create({$name: "Hi"});
assert.strictEqual(myClass.name(), "Hello");
assert.strictEqual(myClass.$name, "Hi");
myClass._free();


const myEnum = $.MyEnum.create($.MyEnum.val2);
assert.strictEqual(myEnum.value(), 100);
assert.strictEqual(myEnum.$value, 1);
myEnum._free();


const myOtherClass = $.MyOtherClass.create({$n: 5});
assert.strictEqual(myOtherClass.n, 5);
assert.strictEqual(myOtherClass.$n, 5);

myOtherClass.$n = 10;
assert.strictEqual(myOtherClass.n, 10);
assert.strictEqual(myOtherClass.$n, 10);

myOtherClass.n = 1024;
assert.strictEqual(myOtherClass.n, 1024);
assert.strictEqual(myOtherClass.$n, 1024);

myOtherClass._free();


$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();