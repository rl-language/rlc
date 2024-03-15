# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t
import bounded_arg

fun main() -> Int:
    let x : BoundedArg<0, 3>
    x.value = 2
    if to_string(x) != "2"s:
        return -1

    let loaded : BoundedArg<0, 3>
    from_string(loaded, "2"s)
    if loaded != x:
        return -2

    let y = 10
    let out = as_byte_vector(y)
    from_byte_vector(loaded, out)
    
    if loaded.value != 1:
        return -2

    return 0
