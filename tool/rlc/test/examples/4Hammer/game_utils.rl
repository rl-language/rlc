fun gen_methods():
    let x : Vector<Bool>
    let action : AnyGameAction
    let board : Board
    for alternative of action:
        pretty_string(board, alternative)
    pretty_string(board, action)
    let profile = make_tantus()
    to_string(Weapon::captain_storm_bolter)
    to_string(Profile::captain_octavius)
    profile.models.size()
    to_string(profile)
    to_string(profile.models[0])
    print(profile)
    to_string(action)
    print(action)
    to_string(enumerate(action))
    to_string(true)
    to_string(CurrentStateDescription::none)
    print(enumerate(action))

fun fuzz(Vector<Byte> input):
    let state = play()
    let x : AnyGameAction
    let enumeration = enumerate(x)
    let index = 0
    while index + 8 < input.size() and !state.is_done():
        let num_action : Int
        from_byte_vector(num_action, input, index)
        if num_action < 0:
          num_action = num_action * -1 
        if num_action < 0:
          num_action = 0 

        let executable : Vector<AnyGameAction>
        let i = 0
        #print("VALIDS")
        while i < enumeration.size():
          if can apply(enumeration.get(i), state):
            #print(enumeration.get(i))
            executable.append(enumeration.get(i))
          i = i + 1
        #print("ENDVALIDS")
        if executable.size() == 0:
            assert(false, "zero valid actions")
            return

        #print(executable.get(num_action % executable.size()))
        apply(executable.get(num_action % executable.size()), state)


fun default_get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    let d : Dice
    d.value = 1
    if can g.roll(d):
        return -1
    if can g.roll_pair(d, d):
        return -1
    if can g.reroll(d):
        return -1
    if can g.quantity(d):
        return -1
    return int(g.board.current_decision_maker)

