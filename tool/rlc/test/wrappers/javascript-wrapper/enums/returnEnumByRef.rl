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

const color = $.Enum_ColorEnum.create($.Enum_ColorEnum.red);
const ref = $.f_foo(color);

ref.value = $.Enum_ColorEnum.blue;
assert.strictEqual(color.value, $.Enum_ColorEnum.blue);
assert.strictEqual(ref.value, $.Enum_ColorEnum.blue);

color._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();