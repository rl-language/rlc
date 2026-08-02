# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls MyClass:
    Int _privateInt
    Int publicInt

    fun _privateFunction():
        self.publicInt = 8

    fun getPrivateInt() ->Int:
        return self._privateInt

    fun publicFunction():
        self._privateFunction()
        self._privateInt = 4


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that private member functions and variables are inaccessible

assert.throws(() => $.MyClass.create({_privateInt: 2}));
assert.throws(() => $.MyClass.create({privateInt: 2}));
assert.throws(() => $.MyClass.create({_privateInt: 2}));

const obj = $.MyClass.create({publicInt: 2});
assert.strictEqual(obj.publicInt, 2);
assert.strictEqual(obj.getPrivateInt(), 0);
assert.strictEqual(obj._privateInt, undefined);
assert.strictEqual(obj.privateInt, undefined);
assert.strictEqual(obj._privateInt, undefined);

obj.publicFunction();
assert.strictEqual(obj.publicInt, 8);
assert.strictEqual(obj.getPrivateInt(), 4);

obj._free();

$._detectMemoryLeaksDoNotUse();