# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
import string
import serialization.print

cls Asd:
    String asd
    Int second

fun main() -> Int:
    let asd : Asd
    asd.asd = "first"s
    asd.second = 5
    if "{asd: \"first\", second: 5}"s != to_string(asd):
        return 2
    let asd2 : Asd
    if !from_string(asd2, to_string(asd)):
        return 1
    return 0
