# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
import collections.vector

fun foo(Vector<Int> vector):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that the standard library can be imported and it works

const vector = $.VectorTint64_tT.create();
for(let i=0; i<100; i++){
    vector.append(i);
}

for(let i=0; i<100; i++){
    assert.strictEqual(vector.get(i).value, i);
}

assert.strictEqual(vector._data, undefined);
assert.strictEqual(vector._size, undefined);
assert.strictEqual(vector._capacity, undefined);



vector._free();
$.Std._detectMemoryLeaksDoNotUse();