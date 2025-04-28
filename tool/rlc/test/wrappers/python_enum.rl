# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared --pyrlc-lib=%pyrlc_lib
# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py

#--- source.rl

enum Something:
    first
    second


#--- to_run.py
import wrapper

assert wrapper.Something.second().value == 1
