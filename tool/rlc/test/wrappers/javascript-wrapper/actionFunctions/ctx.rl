# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act vending_machine(ctx Int target_cost) -> VendingMachine:
    while target_cost != 0:
        act insert_coin(Int coin_value) {coin_value == 1 or coin_value == 5 or coin_value == 10}
        target_cost = target_cost - coin_value

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if "ctx" works

const targetCost = $.Int.create(16);
const machine = $.vending_machine(targetCost);
assert.strictEqual(targetCost.value, 16);

machine.insert_coin(targetCost, 10);
assert.strictEqual(targetCost.value, 6);

machine.insert_coin(targetCost, 5);
assert.strictEqual(targetCost.value, 1);

machine.insert_coin(targetCost, 1);
assert.strictEqual(targetCost.value, 0);

assert.strictEqual(machine.is_done(), true);

machine._free();
targetCost._free();
$._detectMemoryLeaksDoNotUse();