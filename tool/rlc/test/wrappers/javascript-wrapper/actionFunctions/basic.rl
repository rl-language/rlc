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

assert.strictEqual($.f_can_sequence(), true);
const seq = $.f_sequence();
assert.strictEqual(seq.v_resume_index, 1); 
seq.f_first();
assert.strictEqual(seq.f_can_second(1), false);
assert.strictEqual(seq.f_can_second(2), true);
seq.f_second(0);
seq.f_third();
assert.throws(() => seq.f_first());
assert.strictEqual(seq.f_is_done(), true);
seq._free();


//The resume_index should start from 1, not from 0, that's why you should
//avoid creating action functions using the constructor.
const wrongSeq = $.Act_Sequence.create();
assert.strictEqual(wrongSeq.v_resume_index, 0); 
wrongSeq._free();

$._detectMemoryLeaksDoNotUse();