# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared %pyrlc_lib
# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py

#--- source.rl
import python

fun rlc_call(PyObject x, Bool obj):
    x.call(to_pyobject(obj))


#--- to_run.py
import wrapper


called = False
def py_call(obj):
    global called
    called = obj

wrapper.functions.rlc_call(py_call, True)
assert(called)

