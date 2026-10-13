# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --header
# RUN: clang %t/to_run.c -o %t/lib.o -c
# RUN: rlc %t/source.rl %t/lib.o -o %t/result%exeext -i %stdlib  --mutation-testing
# RUN: %t/result%exeext


#--- source.rl
fun to_invoke() -> Int {true}:
  return 5 + 10

#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"
extern int64_t rl_mutation_egine_current_id;

int main() {
  rl_mutation_egine_current_id = 0;
  int64_t result;
  rl_to_invoke__r_int64_t(&result);
  return 115 - result;
}


