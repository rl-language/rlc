# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared --pyrlc-lib=%pyrlc_lib
# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py

#--- source.rl
fun<T> this_one(T i) -> StringLiteral:
    if i is Int:
        return "int"
    if i is Bool:
        return "bool"
    return "none"

fun asd():
    this_one(1)
    this_one(true)

#--- to_run.py
import wrapper

assert(wrapper.this_one(True).decode("utf-8") == 'bool')
