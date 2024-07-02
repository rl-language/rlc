# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import action

fun main() -> Int:
    let vec : Vector<Float>
    let v : BoundedVector<Bool, 5>

    v.append(false)
    v.append(true)
    let x = observation_tensor_size(v)
    print(observation_tensor_size(v))
    while x != 0:
        vec.append(0.0)
        x = x - 1
    to_observation_tensor(v, 0, vec)
    if vec.size() != 5:
        return 1
    if vec.get(0) != 0.0:
        return 2
    if vec.get(1) != 1.0:
        return 3

    return 0
