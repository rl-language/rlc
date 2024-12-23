cls PyObject:
    OwningPtr<Int> _ptr

    fun<Int X> call(StringLiteral name, PyObject[X] args) -> Bool:
        return pyinvoke(self, name, args[0], X)

    fun call(StringLiteral name, PyObject args) -> Bool:
        return pyinvoke(self, name, args, 1)

    fun call(StringLiteral name) -> Bool:
        let x : PyObject
        return pyinvoke(self, name, x, 0)

    fun<Int X> call(PyObject[X] args) -> Bool:
        return pyinvoke(self, args[0], X)

    fun call(PyObject args) -> Bool:
        return pyinvoke(self, args, 1)

    fun drop():
        pyref_decrease(self)

    fun assign(PyObject o):
        self._ptr = o._ptr
        pyref_increase(self)

ext fun pyref_decrease(PyObject self)
ext fun pyref_increase(PyObject self)

ext fun to_pyobject(Int value) -> PyObject 
ext fun to_pyobject(Bool value) -> PyObject 
ext fun to_pyobject(Float value) -> PyObject 

ext fun pyinvoke(PyObject obj, StringLiteral name, PyObject args, Int size) -> Bool
ext fun pyinvoke(PyObject obj, PyObject args, Int size) -> Bool

ext fun to_pyobject(Int obj, StringLiteral str) -> PyObject

ext fun pyprint(PyObject obj)

fun<T> to_pyobject(T obj) -> PyObject:
    let name = __builtin_mangled_name(obj)
    return to_pyobject(__builtin_as_ptr_do_not_use(obj), name) 
