# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
cls BigBox:
    Int value
    Int _secretValue
    SmallBox smallBox

cls SmallBox:
    Int value
    StringLiteral name

    fun init():
        self.value = 0
        self.name = "Unknown"

    fun multiplyValueBy2():
        self.value = self.value * 2

#I write these parameters to generate the corresponding classes
fun foo(Int | BigBox | StringLiteral alt,
    OwningPtr<Int> ptr1,
    OwningPtr<StringLiteral> ptr2,
    OwningPtr<BigBox> ptr3,
    OwningPtr<Int | BigBox | StringLiteral> ptr4,
    OwningPtr<StringLiteral[4]> ptr5):
    1+1

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if I can create pointers correctly


//Pointer to int
let ptr;
let arrSize;
let smallBox;
let bigBox;
let alt;
let arr;

ptr = $.Ptr_Int.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.Ptr_Int.malloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get(), 0);

ptr.set(7);
assert.strictEqual(ptr.get(), 7);

$.free(ptr);
ptr._free();



//Pointer to array of Int
ptr = $.Ptr_Int.create();
assert.strictEqual(ptr.value, 0);


arrSize = 3;
ptr.value = $.Ptr_Int.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i), 0);
}

for (let i = 0; i < arrSize; i++) {
    ptr.set(i, i);
}
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i), i);
}

$.free(ptr, arrSize);
ptr._free();



//Pointer to string
ptr = $.Ptr_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.Ptr_StringLiteral.malloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get(), "");

ptr.set("Hello");
assert.strictEqual(ptr.get(), "Hello");

$.free(ptr);
ptr._free();



//Pointer to array of strings
ptr = $.Ptr_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_StringLiteral.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i), "");
}

for (let i = 0; i < arrSize; i++) {
    ptr.set(String(i), i);
}
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i), String(i));
}
$.free(ptr, arrSize);
ptr._free();



//Pointer to BigBox
ptr = $.Ptr_Cls_BigBox.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.Ptr_Cls_BigBox.malloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get().v_value, 0);
assert.strictEqual(ptr.get().v_smallBox.v_value, 0);
assert.strictEqual(ptr.get().v_smallBox.v_name, "Unknown");

ptr.get().v_smallBox.v_name = "Hello";
assert.strictEqual(ptr.get().v_smallBox.v_name, "Hello");

smallBox = $.Cls_SmallBox.create({ v_value: 5, v_name: "Ciao" });
bigBox = $.Cls_BigBox.create({ v_smallBox: smallBox, v_value: 123 });
ptr.set(bigBox);
assert.strictEqual(ptr.get().v_value, 123);
assert.strictEqual(ptr.get().v_smallBox.v_value, 5);
assert.strictEqual(ptr.get().v_smallBox.v_name, "Ciao");

smallBox._free();
bigBox._free();
$.free(ptr);
ptr._free();



//Pointer to array of BigBox
ptr = $.Ptr_Cls_BigBox.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_Cls_BigBox.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).v_value, 0);
    assert.strictEqual(ptr.get(i).v__secretValue, undefined);
    assert.strictEqual(ptr.get(i).v_smallBox.v_value, 0);
    assert.strictEqual(ptr.get(i).v_smallBox.v_name, "Unknown");
}

bigBox = $.Cls_BigBox.create();
bigBox.v_value = 4;
bigBox.v_smallBox.v_value = 16;
bigBox.v_smallBox.v_name = "Hello";

for (let i = 0; i < arrSize; i++) {
    ptr.set(bigBox, i);
}

for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).v_value, 4);
    assert.strictEqual(ptr.get(i).v__secretValue, undefined);
    assert.strictEqual(ptr.get(i).v_smallBox.v_value, 16);
    assert.strictEqual(ptr.get(i).v_smallBox.v_name, "Hello");
}

bigBox._free();
$.free(ptr, arrSize);
ptr._free();



//Pointer to Alternative
ptr = $.Ptr_Alt3_Int_Cls_BigBox_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.Ptr_Alt3_Int_Cls_BigBox_StringLiteral.malloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get().value, 0);

ptr.get().value = "Hello"
assert.strictEqual(ptr.get().value, "Hello");

alt = $.Alt3_Int_Cls_BigBox_StringLiteral.create("Ciao");
ptr.set(alt);
assert.strictEqual(ptr.get().value, "Ciao");

alt._free();
$.free(ptr);
ptr._free();



//Pointer to array of Alternative
ptr = $.Ptr_Alt3_Int_Cls_BigBox_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_Alt3_Int_Cls_BigBox_StringLiteral.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value, 0);
}

bigBox = $.Cls_BigBox.create();
bigBox.v_value = 4;
bigBox.v_smallBox.v_value = 16;
bigBox.v_smallBox.v_name = "Hello";

alt = $.Alt3_Int_Cls_BigBox_StringLiteral.create(bigBox);

for (let i = 0; i < arrSize; i++) {
    ptr.set(alt, i);
}

alt.value = "Ciao";
ptr.set(alt, 0);

assert.strictEqual(ptr.get(0).value, "Ciao");
for (let i = 1; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value.v_value, 4);
    assert.strictEqual(ptr.get(i).value.v__secretValue, undefined);
    assert.strictEqual(ptr.get(i).value.v_smallBox.v_value, 16);
    assert.strictEqual(ptr.get(i).value.v_smallBox.v_name, "Hello");
}

bigBox._free();
alt._free();
$.free(ptr, arrSize);
ptr._free();



//Pointer to Array
ptr = $.Ptr_Arr_4_StringLiteral.create($.Ptr_Arr_4_StringLiteral.malloc());
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < 4; i++) {
    assert.strictEqual(ptr.get().get(i), "");
}

arr = $.Arr_4_StringLiteral.create(["0", "1", "2", "3"]);
ptr.set(arr);
for (let i = 0; i < 4; i++) {
    assert.strictEqual(ptr.get().get(i), String(i));
}

arr._free();
$.free(ptr);
ptr._free();



//Pointer to array of Array
ptr = $.Ptr_Arr_4_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_Arr_4_StringLiteral.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    for (let j = 0; j < 4; j++) {
        assert.strictEqual(ptr.get(i).get(j), "");
    }
}

arr = $.Arr_4_StringLiteral.create(["0", "1", "2", "3"]);

for (let i = 0; i < arrSize; i++) {
    ptr.set(arr, i);
}
for (let i = 0; i < 4; i++) {
    arr.set(`${String(i)}${String(i)}`, i);
}
ptr.set(arr, 0);
for (let j = 0; j < 4; j++) {
    assert.strictEqual(ptr.get(0).get(j), `${String(j)}${String(j)}`);
}

for (let i = 1; i < arrSize; i++) {
    for (let j = 0; j < 4; j++) {
        assert.strictEqual(ptr.get(i).get(j), String(j));
    }
}

arr._free();
$.free(ptr, arrSize);
ptr._free();




$.StringPool.free();
$._detectMemoryLeaksDoNotUse();