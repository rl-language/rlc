# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act sequence() ->Sequence:
    act first()
    act second(Int i){i % 2 == 0}
    act third()

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if basic action functions work

assert.strictEqual($.can_sequence(), true);
const seq = $.sequence();
assert.strictEqual(seq.resume_index, 1); 
seq.first();
assert.strictEqual(seq.can_second(1), false);
assert.strictEqual(seq.can_second(2), true);
seq.second(0);
seq.third();
assert.throws(() => seq.first());
assert.strictEqual(seq.is_done(), true);
seq._free();


//The resume_index should start from 1, not from 0, that's why you should
//avoid creating action functions using the constructor.
const wrongSeq = $.Sequence.create();
assert.strictEqual(wrongSeq.resume_index, 0); 
wrongSeq._free();

$.Std._detectMemoryLeaksDoNotUse();