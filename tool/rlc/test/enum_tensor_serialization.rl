# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import action
import machine_learning

enum Example:
    first
    second
    third

fun main() -> Int:
    let vec : Vector<Float>
    let value = Example::second

    if !(value is Enum):
        return -5

    let x = observation_tensor_size(value)
    if x != 3:
        return 5
    while x != 0:
        vec.append(0.0)
        x = x - 1
    to_observation_tensor(value, 0, vec)
    if vec.size() != 3:
        return 1
    if vec.get(0) != 0.0:
        return 2
    if vec.get(1) != 1.0:
        return 3
    if vec.get(0) != 0.0:
        return 4

    return 0
