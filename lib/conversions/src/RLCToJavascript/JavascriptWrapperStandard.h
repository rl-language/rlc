#pragma once

namespace mlir::rlc {
    constexpr const char* standardJavascriptWrapper = R"JSWrapper(
import createModule from "./glueCode.mjs";
const module = await createModule();

export function _detectMemoryLeaksDoNotUse() {
    if (typeof module.___lsan_do_leak_check === "function") {
        module.___lsan_do_leak_check();
    }
    else if (typeof module._lsan_do_leak_check === "function") {
        module._lsan_do_leak_check();
    }
    else {
        console.warn("Address sanitizer is not enabled");
    }
}

const garbageCollectorRegistry = new FinalizationRegistry((address) => {
    if (address !== 0) {
        module._free(address);
    }
});

class ObjectWrapper {
    static #authorizedSymbol = Symbol("authorizedSymbol");

    _address;
    _owner;

    constructor(symbol) {
        if (symbol !== ObjectWrapper.#authorizedSymbol) {
            throw new Error("Please don't use the constructor to create an object. Use the static method '.create(...)");
        }
        ObjectWrapper._assertCreatable(this.constructor, ObjectWrapper);
        Object.seal(this);
    }

    static _createEmpty() {
        const myObj = new this(ObjectWrapper.#authorizedSymbol);
        myObj._address = module._malloc(this._getSize());
        garbageCollectorRegistry.register(myObj, myObj._address);
        myObj._owner = true;

        return myObj;
    }

    static clone(toBeCloned) {
        this._assertWrapper(toBeCloned);
        const myObj = this._createEmpty();
        this._assignCopy(myObj._address, toBeCloned);

        return myObj;
    }

    static create(value) {
        const myObj = this._createEmpty();

        try {
            //value !== null && value !== undefined
            if (value != null) {
                myObj._init(value);
            }
            else {
                myObj._initDefault();
            }
        }
        catch (e) {
            myObj._free();
            throw e;
        }

        return myObj;
    }

    static _createUninitialized() {
        const myObj = this._createEmpty();
        myObj._initUninitialized();
        return myObj;
    }

    static _createByRef(address) {
        let myObj = new this(ObjectWrapper.#authorizedSymbol);

        myObj._owner = false;
        myObj._address = address;

        return myObj;
    }

    _assertAddress() {
        if (this._address === 0) {
            throw new Error("Can't call methods on this object, since memory has been freed");
        }
    }

    _get(fieldClass, fieldOffset) {
        this._assertAddress();
        return fieldClass._retrieve(this._address + fieldOffset);
    }

    _set(fieldClass, fieldOffset, value) {
        this._assertAddress();
        fieldClass._assignCopy(this._address + fieldOffset, value);
    }

    _free() {
        if (this._owner === false) {
            return;
        }
        this._assertAddress();

        this._drop();
        module._free(this._address);
        this._address = 0;
    }

    static _assertWrapper(input) {
        if (!(input instanceof this)) {
            throw new Error(`${String(input)} is not an instance of ${String(this.name)}`);
        }
    }

    static _assertCreatable(target, bannedClassRef) {
        if (target === bannedClassRef) {
            throw new Error(`${bannedClassRef.name} is abstract and can't be instantiated.`);
        }
    }

    static _assignCopy(leftAddress, rightValue) {
        throw new Error("Method '_assignCopy()' must be implemented.");
    }

    static _getSize() {
        throw new Error("Method '_getSize()' must be implemented.");
    }

    static _retrieve(address) {
        throw new Error("Method '_retrieve()' must be implemented.");
    }

    static _is(element) {
        throw new Error("Method '_is()' must be implemented.");
    }

    _init(value) {
        throw new Error("Method '_init()' must be implemented.");
    }

    _initDefault() {
        throw new Error("Method '_initDefault()' must be implemented.");
    }

    _initUninitialized() {
        //It's empty, but it can be overridden
    }

    _drop() {
        throw new Error("Method '_drop()' must be implemented.");
    }
}




class CompositeWrapper extends ObjectWrapper {
    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, CompositeWrapper);
    }

    static _assignCopy(leftAddress, rightValue) {

        this._assertWrapper(rightValue);

        const whatWrite = module.HEAP8
            .subarray(rightValue._address, rightValue._address + rightValue.constructor._getSize());

        module.HEAP8.set(whatWrite, leftAddress);
    }

    static _retrieve(address) {
        return this._createByRef(address);
    }

    static _is(element) {
        return (element instanceof this);
    }
}



export class PtrWrapper extends CompositeWrapper {
    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, PtrWrapper);
    }

    static _getSize() {
        return Address._getSize();
    }

    _init(value) {
        this.value = value;
    }

    _initDefault() {
        this._init(0);
    }

    get value() {
        return this._get(Address, 0);
    }

    set value(value) {
        this._set(Address, 0, value);
    }

    _drop() {
        //Always empty
    }

    static malloc(howMany) {
        //howMany === null || howMany === undefined
        if (howMany == null) {
            howMany = 1;
        }
        Int._assertPrimitive(howMany);
        if (howMany <= 0) {
            throw new Error("This number can't be zero or negative");
        }
        const elementClass = this._getElementClass();
        const actualAddress = module._malloc(elementClass._getSize() * howMany);

        for (let i = 0; i < howMany; i++) {
            elementClass._createByRef(actualAddress + elementClass._getSize() * i)._initDefault();
        }

        return actualAddress;
    }

    static _getElementClass() {
        throw new Error("Method '_getElementClass()' must be implemented.");
    }

    #computeActualAddress(i) {
        this._assertAddress();

        //i === null || i === undefined
        if (i == null) {
            i = 0;
        }
        Int._assertPrimitive(i);
        return this.value + this.constructor._getElementClass()._getSize() * i;
    }

    get(i) {
        this._assertAddress();
        return this.constructor._getElementClass()._retrieve(this.#computeActualAddress(i));
    }

    set(value, i) {
        this._assertAddress();
        this.constructor._getElementClass()._assignCopy(this.#computeActualAddress(i), value);
    }
}



export function free(ptr, howMany) {
    PtrWrapper._assertWrapper(ptr);

    //howMany === null || howMany === undefined
    if (howMany == null) {
        howMany = 1;
    }
    Int._assertPrimitive(howMany);
    if (howMany <= 0) {
        throw new Error("This number can't be zero or negative");
    }

    const type = ptr.constructor._getElementClass();
    for (let i = 0; i < howMany; i++) {
        type._createByRef(ptr.value + type._getSize() * i)._drop();
    }

    module._free(ptr.value);
}



class ClassLikeWrapper extends CompositeWrapper {
    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, ClassLikeWrapper);
    }

    f_init() {
        throw new Error("Method 'f_init()' must be implemented.");
    }

    _drop() {
        try {
            if ((typeof this.f_drop) === "function") {
                this.f_drop();
            }
        }
        catch (e) {
            if ((e instanceof NoFunctionFoundError) === false) {
                throw e;
            }
        }
    }
}

class EnumWrapper extends ClassLikeWrapper {
    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, EnumWrapper);
    }

    static _getSize() {
        return 8;
    }

    _init(value) {
        this.f_init();
        this.value = value;
    }

    _initDefault() {
        this._init(0);
    }

    get value() {
        return this._get(Int, 0);
    }

    set value(value) {
        this._set(Int, 0, value);
    }
}

class ClassWrapper extends ClassLikeWrapper {

    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, ClassWrapper);
    }

    static _getMemberFieldNames() {
        throw new Error("Method '_getMemberFieldNames()' must be implemented.");
    }

    _init(value) {
        this.f_init();

        const memberFieldsNames = this.constructor._getMemberFieldNames();
        for (const [k, v] of Object.entries(value)) {
            if(memberFieldsNames.includes(k)){
                this[k] = v;
            }
            else{
                throw new Error(`Property ${k} doesn't exist on this object`)
            }
        }
    }

    _initDefault() {
        this._init({});
    }
}



class AlternativeWrapper extends CompositeWrapper {

    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, AlternativeWrapper);
    }

    static _getAlternativeClasses() {
        throw new Error("Method '_getAlternativeClasses()' must be implemented");
    }

    static _getIndexOffset() {
        throw new Error("Method '_getIndexOffset()' must be implemented");
    }

    _init(value) {
        this._index = -1;
        this.value = value;
    }

    _initDefault() {
        const classes = this.constructor._getAlternativeClasses();
        const defaultObject = classes[0].create();
        this._init(defaultObject);

        //Potential double free if used incorrectly
        defaultObject._free();
    }

    _initUninitialized() {
        this._index = -1;
    }

    _drop() {
        if (this._index === -1) {
            return;
        }

        const classes = this.constructor._getAlternativeClasses();
        const index = this._index;

        if (index < 0 || index >= classes.length) {
            throw new Error(`Invalid element for this alternative: index is ${String(index)}`);
        }

        classes[index]._createByRef(this._address)._drop();
    }

    set _index(index) {
        this._set(Int, this.constructor._getIndexOffset(), index);
    }

    get _index() {
        return this._get(Int, this.constructor._getIndexOffset());
    }

    set value(element) {
        this._assertAddress();
        this._drop();

        const classes = this.constructor._getAlternativeClasses();

        for (let i = 0; i < classes.length; i++) {
            const currentClass = classes[i];

            if (currentClass._is(element)) {
                this._index = i;
                currentClass._assignCopy(this._address, element);
                return;
            }
        }

        throw new Error("This alternative can't accept that element");
    }

    get value() {
        this._assertAddress();

        const index = this._index;
        const classes = this.constructor._getAlternativeClasses();

        if (index < 0 || index >= classes.length) {
            throw new Error(`Invalid element for this alternative: index is ${String(index)}`);
        }

        const currentClass = classes[index];
        return currentClass._retrieve(this._address);
    }
}

class ArrayWrapper extends CompositeWrapper {

    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, ArrayWrapper);
    }

    static _getDimensions() {
        throw new Error("Method '_getDimensions()' must be implemented");
    }

    static _getElementClass() {
        throw new Error("Method '_getElementClass()' must be implemented");
    }

    get length() {
        const result = this.constructor._getDimensions();
        if (result.length === 1) {
            return result[0];
        }

        return result;
    }

    static _assertIndexes(indexes) {
        const condition = indexes.length === this._getDimensions().length;

        if (!condition) {
            throw new Error("Invalid number of indexes");
        }

        for (let i = 0; i < indexes.length; i++) {
            if (indexes[i] < 0 || indexes[i] >= this._getDimensions()[i]) {
                throw new RangeError("Index out of bound");
            }
        }
    }

    static _getLinearLength() {
        let linearLength = 1;
        for (const el of this._getDimensions()) {
            linearLength *= el;
        }
        return linearLength;
    }

    static _getSize() {
        return this._getLinearLength() * this._getElementClass()._getSize();
    }

    _init(arr) {
        if (Array.isArray(arr) === false) {
            throw new Error("Passed an invalid array in the constructor");
        }

        arr = arr.flat(Infinity);
        let linearLength = this.constructor._getLinearLength();

        if (linearLength !== arr.length) {
            throw new Error("Passed an invalid array in the constructor");
        }

        for (let i = 0; i < linearLength; i++) {
            this.constructor._getElementClass()._assignCopy(this.#getAddressByLinearIndex(i), arr[i]);
        }
    }

    _initDefault() {
        const defaultObject = this.constructor._getElementClass().create();

        let linearLength = this.constructor._getLinearLength();
        for (let i = 0; i < linearLength; i++) {
            this.constructor._getElementClass()._assignCopy(this.#getAddressByLinearIndex(i), defaultObject);
        }

        //Potential double free if used incorrectly
        defaultObject._free();
    }

    _drop() {
        let linearLength = this.constructor._getLinearLength();

        for (let i = 0; i < linearLength; i++) {
            this.constructor._getElementClass()._createByRef(this.#getAddressByLinearIndex(i))._drop();
        }
    }

    set(value, indexes) {
        this._assertAddress();

        if (Int._isPrimitive(indexes)) {
            indexes = [indexes];
        }

        this.constructor._assertIndexes(indexes);

        const address = this.#getAddressByLinearIndex(this.constructor._linearizeIndex(indexes));
        this.constructor._getElementClass()._assignCopy(address, value);
    }

    get(...args) {
        this._assertAddress();
        this.constructor._assertIndexes(args);
        return this.constructor._getElementClass()._retrieve(this.#getAddressByLinearIndex(this.constructor._linearizeIndex(args)));
    }

    static _linearizeIndex(indexes) {
        let index = 0;
        for (let i = 0; i < indexes.length; i++) {
            let coefficient = 1;
            for (let j = i + 1; j < indexes.length; j++) {
                coefficient *= this._getDimensions()[j];
            }
            index += coefficient * indexes[i];
        }
        return index;
    }

    #getAddressByLinearIndex(linearIndex) {
        return this._address + linearIndex * this.constructor._getElementClass()._getSize();
    }
}





export class StringPool {
    static #map = new Map();

    constructor() {
        throw new Error("You can't instantiate a StringPool: it's a static class");
    }

    static _writeString(str) {
        StringLiteral._assertPrimitive(str);

        if (this.#map.has(str) === false) {
            this.#map.set(str, module.stringToNewUTF8(str))
        }

        return this.#map.get(str);
    }

    static free() {
        for (const addr of this.#map.values()) {
            module._free(addr);
        }
        this.#map.clear();
    }
}

class PrimitiveWrapper extends ObjectWrapper {

    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, PrimitiveWrapper);
    }

    get value() {
        return this._get(this.constructor, 0);
    }

    set value(value) {
        this._set(this.constructor, 0, value);
    }

    _init(value) {
        this.value = value;
    }

    _initDefault() {
        this._init(this.constructor._getDefaultValue());
    }

    _drop() {
        //Always empty
    }

    static _unbox(value) {
        if (value instanceof this) {
            return value.value;
        }

        return value;
    }

    static _retrieve(address) {
        return this._getValueFromAddress(address);
    }

    static _is(element) {
        if (element instanceof this) {
            element = element.value;
        }

        return this._isPrimitive(element);
    }

    static _assertPrimitive(value) {
        if (this._isPrimitive(value) === false) {
            throw new TypeError(`${String(value)} is not a valid primitive for ${String(this.name)}`);
        }
    }

    static _getValueFromAddress(address) {
        throw new Error("Method '_getValueFromAddress()' must be implemented.");
    }

    static _getDefaultValue() {
        throw new Error("Method '_getDefaultValue()' must be implemented.");
    }

    static _isPrimitive(value) {
        throw new Error("Method '_isPrimitive()' must be implemented.");
    }
}

export class StringLiteral extends PrimitiveWrapper {

    static _getValueFromAddress(address) {
        const actualAddress = module.getValue(address, '*');
        return module.UTF8ToString(actualAddress);
    }

    static _assignCopy(leftAddress, rightValue) {
        rightValue = this._unbox(rightValue);
        const actualAddress = StringPool._writeString(rightValue);
        module.setValue(leftAddress, actualAddress, '*');
    }

    static _getSize() {
        return Address._getSize();
    }

    static _getDefaultValue() {
        return "";
    }

    static _isPrimitive(value) {
        return (typeof value === "string");
    }
}




class NumWrapper extends PrimitiveWrapper {

    constructor(symbol) {
        super(symbol);
        ObjectWrapper._assertCreatable(this.constructor, NumWrapper);
    }

    //BoolWrapper overrides this function
    static _getValueFromAddress(address) {
        return Number(module.getValue(address, this._getStringSize()));
    }

    static _assignCopy(leftAddress, rightValue) {
        rightValue = this._unbox(rightValue);
        this._assertPrimitive(rightValue);
        module.setValue(leftAddress, rightValue, this._getStringSize());
    }

    //BoolWrapper overrides this function
    static _getDefaultValue() {
        return 0;
    }

    static _getStringSize() {
        throw new Error("Method '_getStringSize()' must be implemented.");
    }
}



export class Int extends NumWrapper {
    static _getSize() {
        return 8;
    }

    static _getStringSize() {
        return "i64";
    }

    static _isPrimitive(value) {
        return Number.isInteger(value) || typeof (value) === "bigint";
    }
}

class Address extends NumWrapper {
    static _getSize() {
        return pointerSize;
    }

    static _getStringSize() {
        return "*";
    }

    static _isPrimitive(value) {
        if (value < 0) {
            return false;
        }

        return (this._getSize() === 4 && Number.isInteger(value))
            || (this._getSize() === 8 && Int._isPrimitive(value));
    }
}

export class Bool extends NumWrapper {
    static _getSize() {
        return 1;
    }

    static _getStringSize() {
        return "i8";
    }

    static _getValueFromAddress(address) {
        return Boolean(super._getValueFromAddress(address));
    }

    static _getDefaultValue() {
        return false;
    }

    static _isPrimitive(value) {
        return (typeof value === "boolean");
    }
}

export class Float extends NumWrapper {
    static _getSize() {
        return 8;
    }

    static _getStringSize() {
        return "double";
    }

    static _isPrimitive(value) {
        return (typeof value === "number");
    }
}


export class Byte extends NumWrapper {
    static _getSize() {
        return 1;
    }

    static _getStringSize() {
        return "i8";
    }

    static _isPrimitive(value) {
        return Int._isPrimitive(value);
    }
}

class NoFunctionFoundError extends Error {
    constructor(message) {
        super(message);
        this.name = "NoFunctionFoundError";
    }
}

function generalFunction(actualArgs, signatures) {
    function check(expectedTypes) {
        if (actualArgs.length !== expectedTypes.length) {
            return false;
        }

        const length = actualArgs.length;

        for (let i = 0; i < length; i++) {
            if (expectedTypes[i]._is(actualArgs[i]) === false) {
                return false;
            }
        }

        return true;
    }

    function callFunction(functionName, params, modifiedParams) {
        if (Address._getSize() === 8) {
            params = params.map((x) => BigInt(x));
        }
        module[`_${functionName}`](...params);
        for (const obj of modifiedParams) {
            obj._free();
        }
    }

    for (const signature of signatures) {
        const [functionName, expectedTypes, returnType, isRef] = signature;

        if (check(expectedTypes)) {

            const params = [];
            const modifiedParams = [];

            for (let i = 0; i < actualArgs.length; i++) {
                const currentArg = actualArgs[i];

                if (ObjectWrapper.prototype.isPrototypeOf(currentArg)) {
                    params.push(currentArg._address);
                }
                else {
                    const modified = expectedTypes[i].create(currentArg);
                    modifiedParams.push(modified);
                    params.push(modified._address);
                }
            }

            if (isRef) {
                const currentStackPointer = module.stackSave();
                const resultAddress = module.stackAlloc(Address._getSize());
                params.unshift(resultAddress);
                callFunction(functionName, params, modifiedParams);
                const addressRef = Address._getValueFromAddress(resultAddress);
                const returnedObject = returnType._createByRef(addressRef);
                module.stackRestore(currentStackPointer);
                return returnedObject;
            }

            let returnedObject;

            if (returnType != null) {
                returnedObject = returnType.create();
                params.unshift(returnedObject._address);
            }

            callFunction(functionName, params, modifiedParams);

            if (returnedObject instanceof PrimitiveWrapper) {
                const returnedValue = returnedObject.value;
                returnedObject._free();
                return returnedValue;
            }

            return returnedObject;
        }
    }

    throw new NoFunctionFoundError(`No function found with the given arguments`);
}
)JSWrapper";
}