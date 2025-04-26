# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext

# RUN: rlc %t/source.rl -o %t/wrapper.py -i %stdlib --python
# RUN: python %t/to_run.py 

#--- source.rl
fun to_invoke() -> Int {true}:
  return 5

#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#include "./header.h"

int main() {
  int64_t result;
  rl_to_invoke__r_int64_t(&result);
  return 5 - result;
}


#--- to_run.py
import wrapper as wrapper

assert(wrapper.can_to_invoke())
exit(wrapper.to_invoke() - 5)
