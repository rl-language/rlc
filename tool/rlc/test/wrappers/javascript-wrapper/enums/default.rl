# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
enum ColorEnum:
    red
    green
    blue

    fun getName()->StringLiteral:
        if self.value == ColorEnum::red.value:
            return "Red"
        if self.value == ColorEnum::green.value:
            return "Green"
        if self.value == ColorEnum::blue.value:
            return "Blue"

        return "Unknown color"

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the default value for enums is correct

let color = $.ColorEnum.create();
assert.strictEqual(color.value, $.ColorEnum.red);

color._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();