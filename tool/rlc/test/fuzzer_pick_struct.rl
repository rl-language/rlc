import fuzzer.cpp_functions
import fuzzer.utils

ent Test:
    Int field1
    Int field2

act play() -> Play:
    act uses_struct(Test t) {t.field1 > 0, t.field1 < 5, t.field2 > 7, t.field2 < 16}


