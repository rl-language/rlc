fun something(Int[4] a, Int b):
    b = 0
    let i = 0
    while i != 4:
        b = b + a[i]
        i = i + 1


fun vector_sum(Int[10] a, Int[10] b) -> Int[10]:
    let c : Int[10]
    let i = 0
    while i < 10:
        c[i] = a[i] + b[i]
        i = i + 1
    return c