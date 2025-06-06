# python.rl

## Class PyObject

### Fields

### Methods
- **Function**: `call<X : Int>(StringLiteral name, PyObject[X] args)  -> Bool`
- **Function**: `call(StringLiteral name, PyObject args)  -> Bool`
- **Function**: `call(StringLiteral name)  -> Bool`
- **Function**: `call<X : Int>(PyObject[X] args)  -> Bool`
- **Function**: `call(PyObject args)  -> Bool`
- **Function**: `drop() `
- **Function**: `assign(PyObject o) `

## Free Functions

- **Function**: `pyref_decrease(PyObject self) `
- **Function**: `pyref_increase(PyObject self) `
- **Function**: `to_pyobject(Int value)  -> PyObject`
- **Function**: `to_pyobject(Bool value)  -> PyObject`
- **Function**: `to_pyobject(Float value)  -> PyObject`
- **Function**: `pyinvoke(PyObject obj, StringLiteral name, PyObject args, Int size)  -> Bool`
- **Function**: `pyinvoke(PyObject obj, PyObject args, Int size)  -> Bool`
- **Function**: `to_pyobject(Int obj, StringLiteral str)  -> PyObject`
- **Function**: `pyprint(PyObject obj) `
- **Function**: `to_pyobject<T>(T obj)  -> PyObject`

