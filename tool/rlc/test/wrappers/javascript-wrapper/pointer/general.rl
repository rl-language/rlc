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
ptr = $.Ptr_BigBox_c.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.Ptr_BigBox_c.malloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get().value_v, 0);
assert.strictEqual(ptr.get().smallBox_v.value_v, 0);
assert.strictEqual(ptr.get().smallBox_v.name_v, "Unknown");

ptr.get().smallBox_v.name_v = "Hello";
assert.strictEqual(ptr.get().smallBox_v.name_v, "Hello");

smallBox = $.SmallBox_c.create({ value_v: 5, name_v: "Ciao" });
bigBox = $.BigBox_c.create({ smallBox_v: smallBox, value_v: 123 });
ptr.set(bigBox);
assert.strictEqual(ptr.get().value_v, 123);
assert.strictEqual(ptr.get().smallBox_v.value_v, 5);
assert.strictEqual(ptr.get().smallBox_v.name_v, "Ciao");

smallBox._free();
bigBox._free();
$.free(ptr);
ptr._free();



//Pointer to array of BigBox
ptr = $.Ptr_BigBox_c.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_BigBox_c.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value_v, 0);
    assert.strictEqual(ptr.get(i)._secretValue_v, 0);
    assert.strictEqual(ptr.get(i).smallBox_v.value_v, 0);
    assert.strictEqual(ptr.get(i).smallBox_v.name_v, "Unknown");
}

bigBox = $.BigBox_c.create();
bigBox.value_v = 4;
bigBox._secretValue_v = 8;
bigBox.smallBox_v.value_v = 16;
bigBox.smallBox_v.name_v = "Hello";

for (let i = 0; i < arrSize; i++) {
    ptr.set(bigBox, i);
}

for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value_v, 4);
    assert.strictEqual(ptr.get(i)._secretValue_v, 8);
    assert.strictEqual(ptr.get(i).smallBox_v.value_v, 16);
    assert.strictEqual(ptr.get(i).smallBox_v.name_v, "Hello");
}

bigBox._free();
$.free(ptr, arrSize);
ptr._free();



//Pointer to Alternative
ptr = $.Ptr_Alt3_Int_BigBox_c_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.Ptr_Alt3_Int_BigBox_c_StringLiteral.malloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get().value, 0);

ptr.get().value = "Hello"
assert.strictEqual(ptr.get().value, "Hello");

alt = $.Alt3_Int_BigBox_c_StringLiteral.create("Ciao");
ptr.set(alt);
assert.strictEqual(ptr.get().value, "Ciao");

alt._free();
$.free(ptr);
ptr._free();



//Pointer to array of Alternative
ptr = $.Ptr_Alt3_Int_BigBox_c_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_Alt3_Int_BigBox_c_StringLiteral.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value, 0);
}

bigBox = $.BigBox_c.create();
bigBox.value_v = 4;
bigBox._secretValue_v = 8;
bigBox.smallBox_v.value_v = 16;
bigBox.smallBox_v.name_v = "Hello";

alt = $.Alt3_Int_BigBox_c_StringLiteral.create(bigBox);

for (let i = 0; i < arrSize; i++) {
    ptr.set(alt, i);
}

alt.value = "Ciao";
ptr.set(alt, 0);

assert.strictEqual(ptr.get(0).value, "Ciao");
for (let i = 1; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value.value_v, 4);
    assert.strictEqual(ptr.get(i).value._secretValue_v, 8);
    assert.strictEqual(ptr.get(i).value.smallBox_v.value_v, 16);
    assert.strictEqual(ptr.get(i).value.smallBox_v.name_v, "Hello");
}

bigBox._free();
alt._free();
$.free(ptr, arrSize);
ptr._free();



//Pointer to Array
ptr = $.Ptr_Arr_StringLiteral_4.create($.Ptr_Arr_StringLiteral_4.malloc());
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < 4; i++) {
    assert.strictEqual(ptr.get().get(i), "");
}

arr = $.Arr_StringLiteral_4.create(["0", "1", "2", "3"]);
ptr.set(arr);
for (let i = 0; i < 4; i++) {
    assert.strictEqual(ptr.get().get(i), String(i));
}

arr._free();
$.free(ptr);
ptr._free();



//Pointer to array of Array
ptr = $.Ptr_Arr_StringLiteral_4.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.Ptr_Arr_StringLiteral_4.malloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    for (let j = 0; j < 4; j++) {
        assert.strictEqual(ptr.get(i).get(j), "");
    }
}

arr = $.Arr_StringLiteral_4.create(["0", "1", "2", "3"]);

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