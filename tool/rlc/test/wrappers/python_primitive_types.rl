# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared --pyrlc-lib=%pyrlc_lib
# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py

#--- source.rl
import python
import serialization.print

cls Something:
    Int content

fun rlc_true(PyObject x):
    x.call("py_call", to_pyobject(true))
fun rlc_false(PyObject x):
    x.call("py_call", to_pyobject(false))
fun rlc_double(PyObject x):
    x.call("py_call", to_pyobject(10.0))
fun rlc_int(PyObject x):
    x.call("py_call", to_pyobject(10))


#--- to_run.py
import wrapper

class C:
    def __init__(self):
        self.value = None

    def py_call(self, obj):
        self.value = obj
        

c = C()
wrapper.rlc_true(c)
assert(c.value is True)
wrapper.rlc_false(c)
assert(c.value is False)
wrapper.rlc_double(c)
assert(c.value == 10.0)
wrapper.rlc_int(c)
assert(c.value == 10)
