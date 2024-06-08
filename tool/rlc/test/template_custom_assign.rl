# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

cls<T> CustomAssign:
    Int x

    fun assign(CustomAssign<T> other):
        self.x = other.x + 1


fun main() -> Int:
    let var : CustomAssign<Int>
    let copy : CustomAssign<Int>
    copy = var 
    return copy.x - 1
