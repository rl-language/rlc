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

const vector = $.ClsT_Vector$Int$.create();
for(let i=0; i<100; i++){
    vector.f_append(i);
}

for(let i=0; i<100; i++){
    assert.strictEqual(vector.f_get(i).value, i);
}

assert.strictEqual(vector.v__data, undefined);
assert.strictEqual(vector.v__size, undefined);
assert.strictEqual(vector.v__capacity, undefined);



vector._free();
$._detectMemoryLeaksDoNotUse();