# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t/exec -i %stdlib 
# RUN: cd %t && %t/exec

#--- source.rl
import string
import serialization.print
import action

act example() -> Game:
    act asd(Int x)
    act asdy(Bool x)

fun main() -> Int:
    let state = example()
    let actions_vector : Vector<AnyGameAction> 
    if !load_action_vector_file("./content.txt"s, actions_vector):
        return -3
    if  actions_vector.size() != 2: 
        return -1
    apply(actions_vector.get(0), state)
    apply(actions_vector.get(1), state)
    if !state.is_done():
        return -2
    return 0

#--- content.txt
asd {x: 1}
asdy {x: true}

