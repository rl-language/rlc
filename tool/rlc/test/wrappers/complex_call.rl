# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared -O2

# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py 

#--- source.rl
import action

cls LargeStruct:
  Bool | BInt<0, 10> a
  Bool b
  Bool c
  Bool d
  Bool e

fun make_struct() -> LargeStruct:
  let to_return : LargeStruct
  to_return.a = true
  to_return.b = true
  to_return.e = true
  return to_return

fun gen_stuff():
  let s : LargeStruct
  enumerate(s)



#--- to_run.py
import wrapper as wrapper

struct = wrapper.LargeStruct()
res = wrapper.enumerate(struct)
exit()
