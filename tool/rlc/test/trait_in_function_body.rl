# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

trait<T> HasF:
    fun f(T arg)

cls A:
    Int b

fun f(A a):
   a.b 

fun<HasF G> invoke_twice(G a):
    let d = a
    d.f()
    d.f()

fun main() -> Int:
    let a : A
    if a is HasF :
        return 0
    return -1
