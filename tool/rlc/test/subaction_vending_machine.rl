# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        actions:
            act insert_5_coin()
                target_cost = target_cost - 5
            act insert_1_coin()
                target_cost = target_cost - 1
            act insert_10_coin()
                target_cost = target_cost - 10

act vending_machine_times_two(Int first, Int second) -> VendingMachinePair:
    frm first_machine = vending_machine(first)
    frm second_machine = vending_machine(second)

    while !first_machine.is_done() and !second_machine.is_done():
        subaction first_machine
        subaction second_machine



fun main() -> Int:
    let state = vending_machine_times_two(10, 2)
    state.insert_5_coin()
    state.insert_1_coin()
    state.insert_5_coin()
    state.insert_1_coin()
    if state.is_done():
        return 0
    return 1
