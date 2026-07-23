# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act child() ->Child:
  act childAction1()
  act childAction2()


act parent() ->Parent:
  act parentAction1()
  subaction* lol = child()
  act parentAction2()

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if subaction* statements work

let p;

p = $.f_parent();
p.f_parentAction1();
p.f_childAction1();
p.f_childAction2();
p.f_parentAction2();
assert.strictEqual(p.f_is_done(), true);
p._free();


//This is a bit weird
p = $.f_parent();
p.f_parentAction1();
p.f_childAction1();   
p.v_lol.f_childAction2();
assert.strictEqual(p.v_lol.f_is_done(), true);
assert.strictEqual(p.f_can_parentAction2(), false);
p._free();  


$._detectMemoryLeaksDoNotUse();