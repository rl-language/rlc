# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo1(Int int) ->ref Int:
    return int

fun foo2(Float float) ->ref Float:
    return float

fun foo3(Bool bool) ->ref Bool:
    return bool

fun foo4(Byte byte) ->ref Byte:
    return byte

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if numbers are correctly returned by ref
function check(fun, input, newValue, otherNewValue) {
    const ref = fun(input);
    assert.strictEqual(input._address, ref._address);

    ref.value = newValue;
    assert.strictEqual(input.value, newValue);
    assert.strictEqual(ref.value, newValue);

    input.value = otherNewValue;
    assert.strictEqual(input.value, otherNewValue);
    assert.strictEqual(ref.value, otherNewValue);

    const oldInputAddress = input._address;
    input._free();
    assert.strictEqual(input._address, 0);

    assert.strictEqual(ref._address, oldInputAddress);
}

let input;

input = $.Int.create();
check($.f_foo1, input, 7, 50);

input = $.Float.create();
check($.f_foo2, input, 0.76, -50605.43);

input = $.Bool.create();
check($.f_foo3, input, false, true);

input = $.Byte.create();
check($.f_foo4, input, 16, 32);

$._detectMemoryLeaksDoNotUse();