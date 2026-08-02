# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
import serialization.print

fun foo():
    print("Hello")

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking that the runtime library works

const originalWrite = process.stdout.write;
let capturedOutput = "";

process.stdout.write = (chunk, encoding, callback) => {
    capturedOutput += chunk.toString();
    return true;
};

try {
    $.foo();
}
finally {
    process.stdout.write = originalWrite;
}

assert.strictEqual(capturedOutput, "Hello\n");

$.Std._detectMemoryLeaksDoNotUse();