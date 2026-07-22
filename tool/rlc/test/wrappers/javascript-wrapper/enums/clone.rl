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

//Checking if cloning works

const original = $.Enum_ColorEnum.create($.Enum_ColorEnum.red);
const cloned = $.Enum_ColorEnum.clone(original);
assert.strictEqual(original.value, cloned.value);
original._free();
cloned._free();

const other = $.Enum_ColorEnum.create($.Enum_ColorEnum.green);
const other2 = $.Enum_ColorEnum.create($.Enum_ColorEnum.blue);
other.value = other2.value;
assert.strictEqual(other.value, $.Enum_ColorEnum.blue);
other._free();
other2._free();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();