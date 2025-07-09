# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
cls Asd:
    Int content
    fun get_content() -> Int:
        return self.content

using T = Asd 

cls Tasd:
    T content
    fun checktype() -> Int:
        if self.content is Asd:
            return 0
        return 1
    fun add_asd(T asd):
        self.content = asd


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Tasd tasd;
    Asd asd;
    int64_t tresult;
    int64_t result;
    rl_m_init__Tasd(&tasd);
    rl_m_init__Asd(&asd);
    asd.content.content = 8;

    rl_m_checktype__Tasd_r_int64_t(&tresult, &tasd);
    rl_m_add_asd__Tasd_Asd(&tasd, &asd);
    rl_m_get_content__Asd_r_int64_t(&result, &asd);

    return result - 8 && tresult;
}
