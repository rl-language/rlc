# RUN: python %pyscript/test.py --source %s -i %stdlib --rlc rlc

import string

fun test_emtpy_string() -> Bool:
    let x : String
    return x.size() == 0

fun test_concant() -> Bool:
    let x = string("hey") + string("wey")
    return x == string("heywey") 

