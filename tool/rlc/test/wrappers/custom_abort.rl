# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile --abort-symbol "custom_abort"
# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
fun to_invoke() -> Int:
  assert(false, "message")
  return 5

#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#include "./header.h"
#include "string.h"
#include "stdio.h"

void custom_abort(char* message){
}

int main() {
  int64_t result;
  rl_to_invoke__r_int64_t(&result);
  return 0;
}



