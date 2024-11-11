# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared %pyrlc_lib
# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py

#--- source.rl
import python
import serialization.print

cls Something:
    Int content

fun rlc_call(PyObject x, Something value):
    x.call("py_call", to_pyobject(value))


#--- to_run.py
import wrapper

class C:
    def __init__(self):
        self.value = None

    def py_call(self, obj):
        self.value = obj
        

c = C()
rlc_obj = wrapper.Something()
rlc_obj.content = 10

wrapper.functions.rlc_call(c, rlc_obj)

assert(wrapper.addressof(rlc_obj) == wrapper.addressof(c.value))
