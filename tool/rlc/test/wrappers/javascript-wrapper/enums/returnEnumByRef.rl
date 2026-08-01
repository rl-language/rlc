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

const color = $.ColorEnum.create($.ColorEnum.v_red);
const ref = $.f_foo(color);

ref.value = $.ColorEnum.v_blue;
assert.strictEqual(color.value, $.ColorEnum.v_blue);
assert.strictEqual(ref.value, $.ColorEnum.v_blue);

color._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();