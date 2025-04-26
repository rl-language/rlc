# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared %pyrlc_lib
# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py

#--- source.rl
import python
import serialization.print

fun rlc_call(PyObject x, Int value):
    x.call("py_call", to_pyobject(value))


#--- to_run.py
import wrapper

class C:
    def __init__(self):
        self.value = 3

    def py_call(self, obj):
        self.value = obj
        
c = C()

wrapper.rlc_call(c, 10)
assert(c.value == 10)
