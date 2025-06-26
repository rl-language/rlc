# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
cls Pair:
    Int x 
    Int y

    fun to_invoke() -> Int {true}:
        return self.x + self.y


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
  Pair pair;
  pair.x = 2;
  pair.y = 3;
  int64_t result;
  rl_m_to_invoke__Pair_r_int64_t(&result, pair);
  return 5 - result;
}
