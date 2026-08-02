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

fun foo() ->ColorEnum:
    return ColorEnum::green

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if enums are properly returned by functions

const color = $.foo();
assert.strictEqual(color.value, $.ColorEnum.green);

color._free();

$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();