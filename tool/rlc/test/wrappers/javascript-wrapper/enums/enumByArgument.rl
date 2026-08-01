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

fun foo(ColorEnum color):
    color = ColorEnum::green
    
#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if enums are correctly passed as function arguments

const color = $.ColorEnum.create($.ColorEnum.v_red);
assert.strictEqual(color.value, $.ColorEnum.v_red);

$.f_foo(color);
assert.strictEqual(color.value, $.ColorEnum.v_green);

assert.throws(() => $.f_foo($.ColorEnum.v_green));

color._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();