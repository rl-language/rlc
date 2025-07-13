# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
enum Signal:
    rock
    paper
    scizzor 


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Signal signal;
    rl_m_init__Signal(&signal);
    bool is_enum;
    rl_is_enum__Signal_r_bool(&is_enum, &signal);
    int64_t max_value;
    rl_max__Signal_r_int64_t(&max_value, &signal);
    int64_t value = 5;
    rl_from_int__Signal_int64_t(&signal, &value);
    int64_t result;
    rl_as_int__Signal_r_int64_t(&result, &signal);
    char* str;
    char* s = "scizzor";
    rl_as_string_literal__Signal_r_strlit(&str, &signal);
    return strcmp(str, s) == 0;
}
