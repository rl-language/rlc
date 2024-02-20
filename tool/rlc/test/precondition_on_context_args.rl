# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent EntOne:
    Bool field_one

ent EntTwo:
    Bool field_two

act play(ctx EntOne one, ctx EntTwo two) -> Play:
    one.field_one
    act subact() {one.field_one}

fun main() -> Int:
    return 0
