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

//Checking if the assign function works

let color1 = $.ColorEnum.create($.ColorEnum.red);
let color2 = $.ColorEnum.create($.ColorEnum.green);

color1.assign(color2);
assert.strictEqual(color1.value, $.ColorEnum.green);

color1._free();
color2._free();
$._detectMemoryLeaksDoNotUse();