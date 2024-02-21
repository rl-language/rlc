import fuzzer.cpp_functions
import fuzzer.utils

fun crash() {false}:
    return

ent InnerTest:
    Int c

ent Test:
    Int a
    Int b
    InnerTest inner

act subact(ctx Test context_struct) -> Subact:
    frm subact_frame_var : Int
    subact_frame_var = 250
    act uses_context_struct(Test t1) {
        t1.a == context_struct.b,
        t1.b > context_struct.a,
        t1.b < subact_frame_var,
        t1.inner.c == context_struct.b + t1.b
    }

act play() -> Play:
    frm struct_in_frame : Test
    act uses_struct(Test t) {
        t.a >= 0,
        t.a <= 5,
        t.b >= -10,
        t.a <= 2 or t.b >= 5,
        t.b <= 16
    }
    struct_in_frame = t
    act pick_substruct(InnerTest it)
    subaction*(struct_in_frame) s = subact(struct_in_frame) 
    crash()
