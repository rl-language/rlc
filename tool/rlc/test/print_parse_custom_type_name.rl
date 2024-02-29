# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t

import string
import collections.vector
import serialization.print

ent SomeInnerStruct:
    Bool x

ent SomeStruct:
    Bool | SomeInnerStruct x

fun get_type_name(SomeInnerStruct s) -> StringLiteral:
   return "custom" 

fun main() -> Int:
    let var : SomeStruct
    let var2 : SomeInnerStruct
    var2.x = true
    var.x = var2
    let string = to_string(var)
    print(string)
    let var3 : SomeStruct
    from_string(var3, string)
    print(var3)
    if to_string(var3) == "{x: custom {x: true}}":
        return 0
    return 1
