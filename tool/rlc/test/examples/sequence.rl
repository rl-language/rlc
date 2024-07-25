# RUN: python %pyscript/solve.py %s --stdlib %stdlib --rlc rlc
import action
import bounded_arg

using Dice = BInt<1, 7>

cls AttackProfile:
    BInt<0, 30> models 
    BInt<0, 10> attacks_per_model
    BInt<0, 10> balistic_skill
    BInt<0, 10> strenght
    BInt<0, 10> penetration
    BInt<0, 10> damage 

cls DefenseProfile:
    BInt<0, 10> models 
    BInt<0, 10> thoughness 
    BInt<0, 10> wounds 
    BInt<0, 10> armor_save 
    BInt<0, 10> suffered_wounds
    
fun make_profile1() -> AttackProfile:
    let profile : AttackProfile
    profile.models = 20
    profile.attacks_per_model = 3
    profile.balistic_skill = 4
    profile.strenght = 3
    profile.penetration = 0
    profile.damage = 1
    return profile
    
fun make_profile2() -> AttackProfile:
    let profile : AttackProfile
    profile.models = 20
    profile.attacks_per_model = 2
    profile.balistic_skill = 3
    profile.strenght = 5
    profile.penetration = 2
    profile.damage = 2
    return profile

fun make_target() -> DefenseProfile:
    let profile : DefenseProfile 
    profile.models = 5
    profile.thoughness = 5
    profile.wounds = 3
    profile.armor_save = 3
    return profile

fun required_wound_roll(AttackProfile source, DefenseProfile target) -> Int:
    if source.strenght == target.thoughness:
        return 4

    if source.strenght * 2 < target.thoughness:
        return 6
    
    if source.strenght < target.thoughness:
        return 5

    if source.strenght > target.thoughness * 2:
        return 2
    
    if source.strenght > target.thoughness:
        return 3

    return 0

act single_attack(ctx AttackProfile source, ctx DefenseProfile target) -> Attack:
    act roll(Dice d)
    if d < source.balistic_skill.value:
        return

    act roll(Dice d)
    if d < required_wound_roll(source, target):
        return

    act roll(Dice d)
    if d.value >= (target.armor_save + source.penetration).value:
        return
    
    target.suffered_wounds = target.suffered_wounds + source.damage

    if target.suffered_wounds >= target.wounds:
        target.models = target.models - 1
        target.suffered_wounds = 0

act play() -> Game:
    frm target = make_target()
    frm source : AttackProfile 

    act use_profile1(Bool do_it)
    if do_it:
        source = make_profile1()
    else:
        source = make_profile2()

    frm current_model = 0
    while current_model != source.models.value:
        frm current_attack = 0
        while current_attack != source.attacks_per_model.value:
            subaction* (source, target) attack = single_attack(source, target)
            current_attack = current_attack + 1
        current_model = current_model + 1


fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    let d : Dice
    d.value = 1
    if can g.roll(d):
        return -1
    return 0

fun score(Game g, Int player_id) -> Float:
    if !g.is_done(): 
        return 0.0 
    return float(5 - g.target.models.value)

fun get_num_players() -> Int:
    return 1

fun max_game_lenght() -> Int:
    return 5000

