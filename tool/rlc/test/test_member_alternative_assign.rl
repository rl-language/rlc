# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t

ent SomeInnerStruct:
    Bool x

ent SomeStruct:
    Bool | SomeInnerStruct x


fun main() -> Int:
    let var : SomeStruct
    let var2 : SomeInnerStruct
    var2.x = true
    var.x = var2
    ref field = var.x
    if field is SomeInnerStruct:
        if field.x:   
            return 0
        else:
            return 1
    return -1

