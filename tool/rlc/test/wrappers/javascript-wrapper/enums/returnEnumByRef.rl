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

fun foo(ColorEnum color) ->ref ColorEnum:
    return color

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if enums are properly returned by ref

const color = $.ColorEnum.create($.ColorEnum.red);
const ref = $.foo(color);

ref.value = $.ColorEnum.blue;
assert.strictEqual(color.value, $.ColorEnum.blue);
assert.strictEqual(ref.value, $.ColorEnum.blue);

color._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();