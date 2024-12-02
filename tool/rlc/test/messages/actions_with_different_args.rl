# RUN: rlc %s -o %t --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 7:5: error: Multiple definitions of actions with same name but different argument types  

act guess() -> Guess:
    act p1_choise(frm Int secret)
    act p1_choise(Int guess)	
