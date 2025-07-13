# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
using ThisOne = Int | Bool


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    ThisOne this;
    int64_t x = 3;
    bool y = 1;
    rl_m_assign__ThisOne_int64_t(&this, &x);
    rl_m_assign__ThisOne_bool(&this, &y);
    return (this.content.field0 - 3) && !this.content.field1;
}
