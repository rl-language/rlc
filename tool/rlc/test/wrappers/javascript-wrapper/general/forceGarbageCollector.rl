# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls SmallBox:
    Int value
    StringLiteral name
    Int _secretValue

    fun init():
        self.value = 5
        self.name = "Unknown"

cls BigBox:
    Int value
    SmallBox smallBox

cls Weird:
    OwningPtr<Int> ptr

    fun init():
        self.ptr = __builtin_malloc_do_not_use<Int>(1)
        __builtin_construct_do_not_use(self.ptr[0])
        self.ptr[0] = 5

    fun drop():
        __builtin_destroy_do_not_use(self.ptr[0])
        __builtin_free_do_not_use(self.ptr)

fun foo(Int | SmallBox alt, BigBox[2][3] arr):
    let x : Int | SmallBox
    let y : BigBox[2][3]


#--- test.mjs
import { setImmediate } from 'timers/promises';
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if the garbage collector calls _free on the Javascript objects

function foo(){
    const int = $.Int.create(5);
    const strLit = $.StringLiteral.create("Hello");
    const smallBox = $.SmallBox.create();
    const bigBox = $.BigBox.create();
    const weird = $.Weird.create();
    const alt = $.alt_int64_t_or_SmallBox.create();
    const arr = $.BigBox_2_3.create();
}

foo();

if(global.gc){
    global.gc();
}

//The FinalizationRegistry callback doesn't run immediately with the garbage collector
await setImmediate();

$.StringPool.free();
$._detectMemoryLeaksDoNotUse();