# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls ClassFunctionOverloading:
    Int value

    fun init():
        self.value = 5

    fun foo(Int x) ->Int:
        return x + self.value

    fun foo(Float x) ->Float:
        return x * float(self.value)

    fun foo(Int x, Int y) ->Int:
        return x + y + self.value

    fun foo() -> ref Int:
        return self.value

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that function overloading works for member functions

const obj = $.ClassFunctionOverloading.create();
assert.strictEqual(obj.value, 5);

const int = $.Int.create(5);
assert.strictEqual(obj.foo(int), 10);

const double = $.Float.create(5);
assert.strictEqual(obj.foo(double), 25.0);
assert.strictEqual(obj.foo(0.4), 2.0);

const otherInt = $.Int.create(10);
assert.strictEqual(obj.foo(int, otherInt), 20);
assert.strictEqual(obj.foo(int, 10), 20);
assert.strictEqual(obj.foo(5, otherInt), 20);
assert.strictEqual(obj.foo(5, 10), 20);

const result = obj.foo();
result.value = 32;
assert.strictEqual(obj.value, 32);

obj._free();
int._free();
double._free();
otherInt._free();
$.StringPool.free();

$._detectMemoryLeaksDoNotUse();