# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext | FileCheck %s


import action


cls TopLevel:
    # CHECK: a is of type Int, which is not enumerable. Replace it instead with a BInt with appropriate bounds or specify yourself how to enumerate it.
    Int a
    # CHECK-NEXT: b is of type Float, which is not enumerable. Specify yourself how to enumerate it.
    Float b
    Bool[3] c
    Vector<BInt<0, 3>> f

@classes
act play() -> Game:
    # CHECK: GameDoNothing.a is of type Int, which is not enumerable. Replace it instead with a BInt with appropriate bounds or specify yourself how to enumerate it.
    act do_nothing(Int a, Bool b)

fun main() -> Int:
    let y : TopLevel
    let state : AnyGameAction
    print(get_enumeration_errors(y))
    print(get_enumeration_errors(state))
    return 0
