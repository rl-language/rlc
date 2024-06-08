# RUN: rlc %s -o %t -i %stdlib --sanitize -g
# RUN: %t
import collections.vector

cls Entity:
    Vector<Int> member

fun has_loop():
    let i = 0
    while i < 10:
        let e : Entity
        i = i + 1

fun main() -> Int:
    has_loop()
    return 0
