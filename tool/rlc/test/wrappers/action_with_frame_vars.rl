# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --header
# RUN: clang %t/to_run.c %t/lib%sharedext -o %t/result%exeext
# RUN: %t/result%exeext

# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py 

#--- source.rl
act to_invoke(frm Int arg) -> Frame:
    frm to_return = 4 
    act f()

#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#undef __cplusplus
#include "./header.h"

int main() {
  Frame result;
  int64_t arg = 7;
  rl_to_invoke__int64_t_r_Frame(&result, &arg);
  return 11 - (result.content.to_return + result.content.arg);
}


#--- to_run.py
import wrapper as wrapper

frame = wrapper.functions.to_invoke(7)
exit((frame.to_return + frame.arg) - 11)
