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

    let x : Int | BigBox | StringLiteral
    x = 3
    let y : StringLiteral[4]
    y = y

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

ptr = $.ptr_Int.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.ptr_Int.calloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get(), 0);

ptr.set(7);
assert.strictEqual(ptr.get(), 7);

$.free(ptr);
ptr._free();



//Pointer to array of Int
ptr = $.ptr_Int.create();
assert.strictEqual(ptr.value, 0);


arrSize = 3;
ptr.value = $.ptr_Int.calloc(arrSize);
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
ptr = $.ptr_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.ptr_StringLiteral.calloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get(), "");

ptr.set("Hello");
assert.strictEqual(ptr.get(), "Hello");

$.free(ptr);
ptr._free();



//Pointer to array of strings
ptr = $.ptr_StringLiteral.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.ptr_StringLiteral.calloc(arrSize);
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
ptr = $.ptr_BigBox.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.ptr_BigBox.calloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get().value, 0);
assert.strictEqual(ptr.get().smallBox.value, 0);
assert.strictEqual(ptr.get().smallBox.name, "Unknown");

ptr.get().smallBox.name = "Hello";
assert.strictEqual(ptr.get().smallBox.name, "Hello");

smallBox = $.SmallBox.create({ value: 5, name: "Ciao" });
bigBox = $.BigBox.create({ smallBox: smallBox, value: 123 });
ptr.set(bigBox);
assert.strictEqual(ptr.get().value, 123);
assert.strictEqual(ptr.get().smallBox.value, 5);
assert.strictEqual(ptr.get().smallBox.name, "Ciao");

smallBox._free();
bigBox._free();
$.free(ptr);
ptr._free();



//Pointer to array of BigBox
ptr = $.ptr_BigBox.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.ptr_BigBox.calloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value, 0);
    assert.strictEqual(ptr.get(i)._secretValue, undefined);
    assert.strictEqual(ptr.get(i).smallBox.value, 0);
    assert.strictEqual(ptr.get(i).smallBox.name, "Unknown");
}

bigBox = $.BigBox.create();
bigBox.value = 4;
bigBox.smallBox.value = 16;
bigBox.smallBox.name = "Hello";

for (let i = 0; i < arrSize; i++) {
    ptr.set(bigBox, i);
}

for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value, 4);
    assert.strictEqual(ptr.get(i)._secretValue, undefined);
    assert.strictEqual(ptr.get(i).smallBox.value, 16);
    assert.strictEqual(ptr.get(i).smallBox.name, "Hello");
}

bigBox._free();
$.free(ptr, arrSize);
ptr._free();



//Pointer to Alternative
ptr = $.ptr_alt_int64_t_or_BigBox_or_strlit.create();
assert.strictEqual(ptr.value, 0);

ptr.value = $.ptr_alt_int64_t_or_BigBox_or_strlit.calloc();
assert.notStrictEqual(ptr.value, 0);
assert.strictEqual(ptr.get().value, 0);

ptr.get().value = "Hello"
assert.strictEqual(ptr.get().value, "Hello");

alt = $.alt_int64_t_or_BigBox_or_strlit.create("Ciao");
ptr.set(alt);
assert.strictEqual(ptr.get().value, "Ciao");

alt._free();
$.free(ptr);
ptr._free();



//Pointer to array of Alternative
ptr = $.ptr_alt_int64_t_or_BigBox_or_strlit.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.ptr_alt_int64_t_or_BigBox_or_strlit.calloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value, 0);
}

bigBox = $.BigBox.create();
bigBox.value = 4;
bigBox.smallBox.value = 16;
bigBox.smallBox.name = "Hello";

alt = $.alt_int64_t_or_BigBox_or_strlit.create(bigBox);

for (let i = 0; i < arrSize; i++) {
    ptr.set(alt, i);
}

alt.value = "Ciao";
ptr.set(alt, 0);

assert.strictEqual(ptr.get(0).value, "Ciao");
for (let i = 1; i < arrSize; i++) {
    assert.strictEqual(ptr.get(i).value.value, 4);
    assert.strictEqual(ptr.get(i).value._secretValue, undefined);
    assert.strictEqual(ptr.get(i).value.smallBox.value, 16);
    assert.strictEqual(ptr.get(i).value.smallBox.name, "Hello");
}

bigBox._free();
alt._free();
$.free(ptr, arrSize);
ptr._free();



//Pointer to Array
ptr = $.ptr_strlit_4.create($.ptr_strlit_4.calloc());
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < 4; i++) {
    assert.strictEqual(ptr.get().get(i), "");
}

arr = $.strlit_4.create(["0", "1", "2", "3"]);
ptr.set(arr);
for (let i = 0; i < 4; i++) {
    assert.strictEqual(ptr.get().get(i), String(i));
}

arr._free();
$.free(ptr);
ptr._free();



//Pointer to array of Array
ptr = $.ptr_strlit_4.create();
assert.strictEqual(ptr.value, 0);

arrSize = 3;
ptr.value = $.ptr_strlit_4.calloc(arrSize);
assert.notStrictEqual(ptr.value, 0);
for (let i = 0; i < arrSize; i++) {
    for (let j = 0; j < 4; j++) {
        assert.strictEqual(ptr.get(i).get(j), "");
    }
}

arr = $.strlit_4.create(["0", "1", "2", "3"]);

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