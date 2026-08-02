# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls Cls1:
    Byte byte
    Int int

cls Cls2:
    Byte byte
    StringLiteral str
    Byte[5] arrByte

cls WeirdStruct:
    Cls1 a
    Byte b
    StringLiteral c
    Byte[13] | StringLiteral d
    Byte[3] e
    Cls1[2] f
    Byte g
    StringLiteral h

fun foo(WeirdStruct s):
    s.a.byte = byte(10)
    s.a.int = 100
    s.b = byte(-50)
    s.c = "Hello"
    s.d = "Ciao"
    s.e[0] = byte(0)
    s.e[1] = byte(1)
    s.e[2] = byte(2)
    s.f[0].int = 60
    s.f[1].int = 120
    s.g = byte(8)
    s.h = "Super"


#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if memory offsets are computed correctly

const weird = $.WeirdStruct.create();

$.foo(weird);
assert.strictEqual(weird.a.byte, 10);
assert.strictEqual(weird.a.int, 100);
assert.strictEqual(weird.b, -50);
assert.strictEqual(weird.c, "Hello");
assert.strictEqual(weird.d.value, "Ciao");
assert.strictEqual(weird.e.get(0), 0);
assert.strictEqual(weird.e.get(1), 1);
assert.strictEqual(weird.e.get(2), 2);
assert.strictEqual(weird.f.get(0).int, 60);
assert.strictEqual(weird.f.get(1).int, 120);
assert.strictEqual(weird.g, 8);
assert.strictEqual(weird.h, "Super");

weird._free();
$.Std.StringPool.free();
$.Std._detectMemoryLeaksDoNotUse();