# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        actions:
            act insert_5_coin()
                target_cost = target_cost - 5
            act insert_1_coin()
                target_cost = target_cost - 1
            act insert_10_coin()
                target_cost = target_cost - 10

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if action statements work

const targetCost = $.Int.create(16);
const machine = $.vending_machine(targetCost);
assert.strictEqual(machine.target_cost, 16);
assert.strictEqual(targetCost.value, 16);
assert.strictEqual(machine.is_done(), false);

machine.insert_10_coin();
assert.strictEqual(machine.target_cost, 6);
assert.strictEqual(targetCost.value, 16);
assert.strictEqual(machine.is_done(), false);

machine.insert_5_coin();
assert.strictEqual(machine.target_cost, 1);
assert.strictEqual(targetCost.value, 16);
assert.strictEqual(machine.is_done(), false);

machine.insert_1_coin();
assert.strictEqual(machine.target_cost, 0);
assert.strictEqual(targetCost.value, 16);
assert.strictEqual(machine.is_done(), true);


targetCost._free();
machine._free();
$._detectMemoryLeaksDoNotUse();