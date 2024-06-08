# RUN: split-file %s %t 
# RUN: python %pyscript/action.py %t/source.rl %t/trace.txt -i %stdlib -ii | FileCheck %s

# CHECK: asd {x: 4} 
# CHECK-NEXT: asd2 {y: {field: 5}}
# CHECK-NEXT: asd3 {}
# CHECK-NEXT: {resume_index: -1}

#--- source.rl
import action

cls Struct:
    Int field

act play() -> Game:
    act asd(Int x) {x == 4}
    act asd2(Struct y) {y.field == 5}
    act asd3()

fun gen_printer_parser():
    let state : Game 
    let any_action :  AnyGameAction
    gen_python_methods(state, any_action)

#--- trace.txt
asd {x: 10}
asd {x: 4}
asd2 {y: {field: 5}}
asd {x: 10}
asd3 {}
