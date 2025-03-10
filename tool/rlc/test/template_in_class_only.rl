# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls<T> Vector:
    Int content

    fun init():
        self.content = 3

    fun drop():
        self.content = 3

cls VectorContainer:
    Vector<Int> asd


fun main() -> Int:
    return 0
