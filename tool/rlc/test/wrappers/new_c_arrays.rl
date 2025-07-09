# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/header.h -i %stdlib --new-header
# RUN: clang %t/to_run.c %t/lib%libext -o %t/result%exeext
# RUN: %t/result%exeext


#--- source.rl
const NODE_SIZE = 1024

cls Graph:
    Bool[NODE_SIZE] nodes

    fun get_value(Int i) -> Bool:
        return self.nodes[i]
    fun set_value(Int i, Bool value) -> Bool {i < 4}:
        self.nodes[i] = value
        return self.nodes[i]



#--- to_run.c
#include <stdint.h>
#include <stdbool.h>
#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "./header.h"

int main() {
    Graph graph;
    int64_t i = 0;
    bool val = true;
    bool result_set1;
    bool result_canset;
    bool result_get1;
    

    rl_m_init__Graph(&graph);

    rl_m_set_value__Graph_int64_t_bool_r_bool(&result_set1, &graph, &i, &val);
    rl_m_get_value__Graph_int64_t_r_bool(&result_get1, &graph, &i); //retrun 1
    
    i = 10;
    rl_m_can_set_value__Graph_int64_t_bool_r_bool(&result_canset, &graph, &i, &val); //retrun 0

    return result_canset && !result_get1; //retrun 0
    
}
