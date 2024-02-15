import fuzzer.cpp_functions
import fuzzer.utils

ent Test:
    Int a
    Int b

act play() -> Play:
    act uses_struct(Test t) {
        t.a >= 0,
        t.a <= 5,
        t.b >= -10,
        t.a <= 2 or t.b >= 5,
        t.b <= 16
    }
