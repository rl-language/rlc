# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import action
import machine_learning

fun main() -> Int:
    let vec : Vector<Float>
    let hidden : HiddenInformation<Bool>
    let hidden2 : Hidden<Bool>

    if observation_tensor_size(hidden2) != 0:
        return 4

    hidden.owner = 1
    hidden.value = true
    let x = observation_tensor_size(hidden)
    while x != 0:
        vec.append(0.0)
        x = x - 1
    to_observation_tensor(hidden, 0, vec)
    if vec.size() != 1:
        return 1
    if vec.get(0) != 0.0:
        return 2
    to_observation_tensor(hidden, 1, vec)
    if vec.get(0) != 1.0:
        return 3

    return 0
