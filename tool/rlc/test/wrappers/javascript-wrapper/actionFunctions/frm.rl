# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        act insert_coin(Int coin_value) {coin_value == 1 or coin_value == 5 or coin_value == 10}
        target_cost = target_cost - coin_value

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if "frm" works

const machine = $.f_vending_machine(16);
assert.strictEqual(machine.v_target_cost, 16);

machine.f_insert_coin(10);
assert.strictEqual(machine.v_target_cost, 6);

machine.f_insert_coin(5);
assert.strictEqual(machine.v_target_cost, 1);

machine.f_insert_coin(1);
assert.strictEqual(machine.v_target_cost, 0);

assert.strictEqual(machine.f_is_done(), true);

machine._free();
$._detectMemoryLeaksDoNotUse();