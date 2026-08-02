# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls ByteStruct:
    Byte byte

cls OtherByteStruct:
    Byte byte1
    Int int
    Byte byte2

cls WeirdStruct:
    Byte a
    Byte[3] b
    Int c
    ByteStruct d
    Byte[17] | Int e
    Byte f
    OtherByteStruct[2] g
    Bool h

fun foo(WeirdStruct s):
    s.a = byte(64)
    s.b[0] = byte(96)
    s.b[1] = byte(-54)
    s.b[2] = byte(100)
    s.c = 4933498
    s.d.byte = byte(6)
    s.e = 32923
    s.f = byte(-2)
    s.g[0].int = 50
    s.g[1].int = 100
    s.h = true

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if memory offsets are computed correctly

const weird = $.WeirdStruct.create();
assert.strictEqual(weird.constructor._getSize(), 120);

$.foo(weird);
assert.strictEqual(weird.a, 64);
assert.strictEqual(weird.b.get(0), 96);
assert.strictEqual(weird.b.get(1), -54);
assert.strictEqual(weird.b.get(2), 100);
assert.strictEqual(weird.c, 4933498);
assert.strictEqual(weird.d.byte, 6);
assert.strictEqual(weird.e.value, 32923);
assert.strictEqual(weird.f, -2);
assert.strictEqual(weird.g.get(0).int, 50);
assert.strictEqual(weird.g.get(1).int, 100);
assert.strictEqual(weird.h, true);

weird._free();
$._detectMemoryLeaksDoNotUse();