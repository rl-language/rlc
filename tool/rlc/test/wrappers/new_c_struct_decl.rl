# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
cls Pair:
    Int x 
    Int y

    fun to_invoke() -> Int {self.x < 10}:
        return self.x + self.y

cls Outer:
    Pair inner

    fun to_invoke() -> Int {self.inner.y > 5}:
        return self.inner.to_invoke()



#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Outer outer;
    int64_t result = 0;
    bool can = true;

    rl_m_init__Outer(&outer);
    outer.content.inner.content.x = 3;
    outer.content.inner.content.y = 4;

    rl_m_can_to_invoke__Outer_r_bool(&can, &outer);
    if(!can){
        outer.content.inner.content.y = 7;
        rl_m_can_to_invoke__Outer_r_bool(&can, &outer);
        if (can) rl_m_to_invoke__Outer_r_int64_t(&result, &outer); //10
    }else {
        rl_m_to_invoke__Outer_r_int64_t(&result, &outer) // 7
    }
    
    return result - 10; // result should be 10
    
}
