# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
act play() -> Game:
    frm asd = 0
    act pick(Int x)
    asd = x


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Game pair; 
    rl_play__r_Game(&pair);
    int64_t x = 3;
    rl_m_pick__Game_int64_t(&pair, &x);
    return pair.content.asd - 3;
}
