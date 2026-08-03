# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act vending_machine(ctx Int target_cost) -> VendingMachine:
    while target_cost != 0:
        actions:
            act insert_5_coin()
                target_cost = target_cost - 5
            act insert_1_coin()
                target_cost = target_cost - 1
            act insert_10_coin()
                target_cost = target_cost - 10

act vending_machine_times_two(frm Int target) -> VendingMachinePair:
    subaction*(target) first_machine = vending_machine(target)
    subaction*(target) second_machine = vending_machine(target)

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if subaction* statements work

const pair = $.vending_machine_times_two(16);
assert.strictEqual(pair.target, 16);
assert.strictEqual(pair.is_done(), false);

pair.insert_5_coin();
assert.strictEqual(pair.target, 11);
assert.strictEqual(pair.is_done(), false);

pair.insert_10_coin();
assert.strictEqual(pair.target, 1);
assert.strictEqual(pair.is_done(), false);

pair.insert_1_coin();
assert.strictEqual(pair.target, 0);
assert.strictEqual(pair.is_done(), true);

pair._free();
$.Std._detectMemoryLeaksDoNotUse();