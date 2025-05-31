# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import collections.vector
import serialization.print

cls Context:
    Int x
    Int y

act sequence(ctx Context context) -> Sequence:
    frm accumulator : Vector<Int> 
    while true:
        act add(Int z)
        accumulator.append(context.x + context.y + z)
        print(accumulator)

fun main() -> Int:
    let context : Context 
    let state = sequence(context)
    state.add(context, 10)
    return 0 
