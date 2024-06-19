# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/exec -i %stdlib 
# RUN: cd %t && %t/exec

#--- source.rl
import string
import serialization.print

fun main() -> Int:
    let content : String
    if !load_file("./content.txt"s, content):
        return -2

    if  content != "example\nex2\n":
        return -1
    return 0

#--- content.txt
example
ex2
