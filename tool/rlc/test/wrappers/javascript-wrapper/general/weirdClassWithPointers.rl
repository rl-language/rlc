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

const weird = $.Cls_WeirdStruct.create();

$.f_foo(weird);
assert.strictEqual(weird.v_a.v_byte, 10);
assert.strictEqual(weird.v_a.v_int, 100);
assert.strictEqual(weird.v_b, -50);
assert.strictEqual(weird.v_c, "Hello");
assert.strictEqual(weird.v_d.value, "Ciao");
assert.strictEqual(weird.v_e.get(0), 0);
assert.strictEqual(weird.v_e.get(1), 1);
assert.strictEqual(weird.v_e.get(2), 2);
assert.strictEqual(weird.v_f.get(0).v_int, 60);
assert.strictEqual(weird.v_f.get(1).v_int, 120);
assert.strictEqual(weird.v_g, 8);
assert.strictEqual(weird.v_h, "Super");

weird._free();
$.StringPool.free();
$._detectMemoryLeaksDoNotUse();