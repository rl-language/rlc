# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
import collections.vector 
cls Pair:
    Vector<Vector<Int>> x

fun asd():
    let x : Pair
    x.x.size()


#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Pair pair;
    VectorTint64_tT inner1;
    VectorTint64_tT inner2;
    int64_t value = 5;
    int64_t result;

    rl_m_init__Pair(&pair);

    rl_m_init__VectorTint64_tT(&inner1);
    rl_m_init__VectorTint64_tT(&inner2);

    rl_m_append__VectorTint64_tT_int64_t(&inner1, &value);
    rl_m_append__VectorTint64_tT_int64_t(&inner1, &value);
    rl_m_append__VectorTint64_tT_int64_t(&inner1, &value);
    rl_m_append__VectorTint64_tT_int64_t(&inner2, &value);
    rl_m_append__VectorTint64_tT_int64_t(&inner2, &value);
    rl_m_append__VectorTint64_tT_int64_t(&inner2, &value);
    rl_m_append__VectorTVectorTint64_tTT_VectorTint64_tT(&pair.content.x, &inner1);
    rl_m_append__VectorTVectorTint64_tTT_VectorTint64_tT(&pair.content.x, &inner2);
    rl_m_size__VectorTVectorTint64_tTT_r_int64_t(&result, &pair.content.x);
    return result - 2;
}
