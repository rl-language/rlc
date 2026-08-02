# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
fun foo1(StringLiteral[4] arr):
    arr[0] = "W"
    arr[1] = "X"
    arr[2] = "Y"
    arr[3] = "Z"

fun foo2(StringLiteral[4] arr):
    let other : StringLiteral[4]
    other[0] = "W"
    other[1] = "X"
    other[2] = "Y"
    other[3] = "Z"
    arr = other

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if arrays are correctly passed as function arguments

function check(foo) {
    const arr = $.strlit_4.create(["A", "B", "C", "D"]);
    foo(arr);
    assert.strictEqual(arr.get(0), "W");
    assert.strictEqual(arr.get(1), "X");
    assert.strictEqual(arr.get(2), "Y");
    assert.strictEqual(arr.get(3), "Z");

    let int = $.Int.create(3);
    assert.throws(() => foo(int));

    int._free();
    arr._free();
}

check($.f_foo1);
check($.f_foo2);

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();