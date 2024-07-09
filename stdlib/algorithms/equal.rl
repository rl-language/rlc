trait<T> Comparable:
    fun custom_equal(T lhs, T rhs) -> Bool

fun custom_equal(Int lhs, Int rhs) -> Bool:
    return lhs == rhs

fun custom_equal(Bool lhs, Bool rhs) -> Bool:
    return lhs == rhs

fun custom_equal(Byte lhs, Byte rhs) -> Bool:
    return lhs == rhs

fun custom_equal(Float lhs, Float rhs) -> Bool:
    return lhs == rhs

fun<T, Int X> custom_equal(T[X] lhs, T[X] rhs) -> Bool:
    let counter = 0
    while counter < X:
        if !equal(lhs, rhs):
            return false
        counter = counter + 1
    return true

# Nof fully impelemted, do not use
fun<T> equal(T lhs, T rhs) -> Bool:
    if lhs is Comparable:
        if rhs is Comparable:
            return custom_equal(lhs, rhs)
    for field, field2 of lhs, rhs:
        using T2 = type(field)
        if field2 is T2:
          if !equal(field, field2):
            return false
    return true
