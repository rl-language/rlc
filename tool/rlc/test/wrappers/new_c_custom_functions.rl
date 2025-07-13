# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
cls Pair:
    Int x 
    Int y

    fun init():
        self.x = 2
        self.y = 1

cls Outer:
    Pair inner


fun to_invoke(Outer self, Int to_add) -> Int {true}:
    return self.inner.x + self.inner.y + to_add


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Outer pair;
    int64_t result;
    int64_t to_add = 5;
    rl_m_init__Outer(&pair);
    pair.content.inner.content.x = 4;
    rl_to_invoke__Outer_int64_t_r_int64_t(&result, &pair, &to_add);
    return result - 10;
}
