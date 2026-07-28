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

//Checking if I can access the value of the enum and call member functions

const color = $.Enum_ColorEnum.create($.Enum_ColorEnum.v_red);
assert.strictEqual(color.value, $.Enum_ColorEnum.v_red);

color.value = 43;
assert.strictEqual(color.value, 43);

const name = color.f_getName();
assert.strictEqual(name, "Unknown color");

color._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();