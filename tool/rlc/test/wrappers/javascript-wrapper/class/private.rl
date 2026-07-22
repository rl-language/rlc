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

assert.throws(() => $.Cls_MyClass.create({v__privateInt: 2}));
assert.throws(() => $.Cls_MyClass.create({v_privateInt: 2}));
assert.throws(() => $.Cls_MyClass.create({_privateInt: 2}));

const obj = $.Cls_MyClass.create({v_publicInt: 2});
assert.strictEqual(obj.v_publicInt, 2);
assert.strictEqual(obj.f_getPrivateInt(), 0);
assert.strictEqual(obj.v__privateInt, undefined);
assert.strictEqual(obj.v_privateInt, undefined);
assert.strictEqual(obj._privateInt, undefined);

obj.f_publicFunction();
assert.strictEqual(obj.v_publicInt, 8);
assert.strictEqual(obj.f_getPrivateInt(), 4);

obj._free();

$._detectMemoryLeaksDoNotUse();