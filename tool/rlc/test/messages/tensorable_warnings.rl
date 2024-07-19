# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext | FileCheck %s


import action


cls TopLevel:
    # CHECK: obj.a is of type Int, which is not tensorable. Replace it instead with a BInt with appropriate bounds or specify yourself how to serialize it,
    Int a
    # CHECK-NEXT: obj.b is of type Float, which is not tensorable. Specify yourself how to serialize it,
    Float b
    Bool[3] c
    Vector<BInt<0, 3>> f

act play() -> Game:
    # CHECK: obj.a2 is of type Int, which is not tensorable. Replace it instead with a BInt with appropriate bounds or specify yourself how to serialize it,
    frm a2 = 0
    act do_nothing()
    frm b2 = true

fun main() -> Int:
    let y : TopLevel
    let state = play()
    print(to_observation_tensor_warnings(y))
    print(to_observation_tensor_warnings(state))
    return 0
