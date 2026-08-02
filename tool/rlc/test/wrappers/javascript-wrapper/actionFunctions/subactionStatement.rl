# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act child() ->Child:
  act childAction1()
  act childAction2()


act parent() ->Parent:
  act parentAction1()
  subaction lol = child()
  act parentAction2()

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if subaction statements work

let p;

p = $.parent();
p.parentAction1();
p.childAction1();
p.parentAction2();
assert.strictEqual(p.is_done(), true);
p._free();


//This is a bit weird
p = $.parent();
p.parentAction1();
p.lol.childAction1();
p.childAction2();
p.parentAction2();
assert.strictEqual(p.is_done(), true);
p._free();


$.Std._detectMemoryLeaksDoNotUse();