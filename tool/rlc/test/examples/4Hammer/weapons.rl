import bounded_arg
import vector2d

using Dice = BInt<1, 7>
using Stat = LinearlyDistributedInt<0, 20> | Dice 

fun raw_stat(Int value) ->  Stat:
    let to_set : LinearlyDistributedInt<0, 20>
    to_set = value
    let stat : Stat 
    stat = to_set
    return stat

fun dice_stat(Int value) ->  Stat:
    let stat : Stat 
    let dice : Dice
    dice.value = value
    stat = dice
    return stat

enum WeaponRuleKind:
    pistol
    ignore_cover
    torrent
    rapid_fire
    psychic
    devastating_wounds
    sustained_hit
    letal_hits 
    precision
    hazardous
    anti_monster
    anti_vehichle
    anti_psychic
    blast
    lance
    extra_attacks
    assault
    heavy
    melta

    fun equal(WeaponRuleKind other) -> Bool:
        return self.value == other.value

fun write_in_observation_tensor(WeaponRuleKind obj, Int observer_id, Vector<Float> output, Int index):
    output[index] = (float(obj.value) - (float(max(obj)) / 2.0)) / float(max(obj))
    index = index + 1

fun size_as_observation_tensor(WeaponRuleKind kind) -> Int:
    return 1

cls WeaponRule:
    WeaponRuleKind kind
    Int parameter

fun weapon_rule(WeaponRuleKind kind, Int parameter) -> WeaponRule:
    let rule : WeaponRule
    rule.kind = kind
    rule.parameter = parameter
    return rule

fun weapon_rule(WeaponRuleKind kind) -> WeaponRule:
    let rule : WeaponRule
    rule.kind = kind
    return rule
    

using WeaponRules = Vector<WeaponRule>

fun no_weapon_rules() -> WeaponRules:
    let to_return : WeaponRules
    return to_return

fun weapon_rules() -> WeaponRules:
    let to_return : WeaponRules
    return to_return
fun weapon_rules(WeaponRule rule) -> WeaponRules:
    let to_return : WeaponRules
    to_return.append(rule)
    return to_return
fun weapon_rules(WeaponRule rule, WeaponRule rule2) -> WeaponRules:
    let to_return : WeaponRules
    to_return.append(rule)
    to_return.append(rule2)
    return to_return
fun weapon_rules(WeaponRule rule, WeaponRule rule2, WeaponRule rule3) -> WeaponRules:
    let to_return : WeaponRules
    to_return.append(rule)
    to_return.append(rule2)
    to_return.append(rule3)
    return to_return

fun weapon_rules(WeaponRule rule, WeaponRule rule2, WeaponRule rule3, WeaponRule rule4) -> WeaponRules:
    let to_return : WeaponRules
    to_return.append(rule)
    to_return.append(rule2)
    to_return.append(rule3)
    to_return.append(rule4)
    return to_return

fun weapon_rules(WeaponRule rule, WeaponRule rule2, WeaponRule rule3, WeaponRule rule4, WeaponRule rule5) -> WeaponRules:
    let to_return : WeaponRules
    to_return.append(rule)
    to_return.append(rule2)
    to_return.append(rule3)
    to_return.append(rule4)
    to_return.append(rule5)
    return to_return

enum Weapon:
    captain_storm_bolter:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 2
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::rapid_fire, 2))
    relic_weapon:
        Int range = 0
        Stat attacks = raw_stat(6)
        Int skill = 2
        Int strenght = 5
        Int penetration = 2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::letal_hits), weapon_rule(WeaponRuleKind::precision))
    smite_witchfire:
        Int range = 24
        Stat attacks = dice_stat(6)
        Int skill = 3
        Int strenght = 5
        Int penetration = 1
        Stat damage = dice_stat(3)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::psychic))
    smite_focused_witchfire:
        Int range = 24
        Stat attacks = dice_stat(6)
        Int skill = 3
        Int strenght = 4
        Int penetration = 2
        Stat damage = dice_stat(3)
        WeaponRules rules = weapon_rules(
                        weapon_rule(WeaponRuleKind::psychic), 
                        weapon_rule(WeaponRuleKind::hazardous), 
                        weapon_rule(WeaponRuleKind::devastating_wounds))
    librarian_storm_bolter:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::rapid_fire, 2))
    force_weapon:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 6
        Int penetration = 1
        Stat damage = dice_stat(3)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::psychic))
    bolt_pistol:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 3
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))
    pyreblaster:
        Int range = 12
        Stat attacks = dice_stat(6)
        Int skill = 0
        Int strenght = 5
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(
                                weapon_rule(WeaponRuleKind::ignore_cover), 
                                weapon_rule(WeaponRuleKind::torrent))
    close_combact_weapon:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 3
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    assault_cannon:
        Int range = 24
        Stat attacks = raw_stat(6)
        Int skill = 3
        Int strenght = 6
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::devastating_wounds))
    terminator_storm_bolter:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::rapid_fire, 2))
    terminator_power_fist:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 3
        Int strenght = 8
        Int penetration = 2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()
    terminator_power_weapon:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 5
        Int penetration = 2
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    death_shadow_claws_and_talons:
        Int range = 0
        Stat attacks = raw_stat(6)
        Int skill = 2
        Int strenght = 6
        Int penetration = 2
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::precision))
    lictor_claws_and_talons:
        Int range = 0
        Stat attacks = raw_stat(6)
        Int skill = 2
        Int strenght = 7
        Int penetration = 2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::precision))
    leapers_talon:
        Int range = 0
        Stat attacks = raw_stat(6)
        Int skill = 3
        Int strenght = 5
        Int penetration = 1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    rite_of_possession:
        Int range = 18
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 4
        Int penetration = -3
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
                                weapon_rule(WeaponRuleKind::psychic), 
                                weapon_rule(WeaponRuleKind::anti_psychic), 
                                weapon_rule(WeaponRuleKind::pistol), 
                                weapon_rule(WeaponRuleKind::precision))
    rite_of_possession_focused:
        Int range = 18
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 6
        Int penetration = 2
        Stat damage = raw_stat(3)
        WeaponRules rules = weapon_rules(
                                weapon_rule(WeaponRuleKind::psychic), 
                                weapon_rule(WeaponRuleKind::hazardous), 
                                weapon_rule(WeaponRuleKind::anti_psychic), 
                                weapon_rule(WeaponRuleKind::pistol), 
                                weapon_rule(WeaponRuleKind::precision))
    staff_of_possession:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 6
        Int penetration = 1
        Stat damage = dice_stat(3)
        WeaponRules rules = weapon_rules(
                                weapon_rule(WeaponRuleKind::psychic), 
                                weapon_rule(WeaponRuleKind::anti_psychic))
    hideous_mutations:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 5
        Int penetration = 1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()
    boltgun:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    heavy_bolter:
        Int range = 36
        Stat attacks = raw_stat(3)
        Int skill = 4
        Int strenght = 5
        Int penetration = 1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::heavy),
            weapon_rule(WeaponRuleKind::sustained_hit, 1)
        )
    meltagun:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 3
        Int strenght = 9
        Int penetration = -4
        Stat damage = dice_stat(6)  
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::melta, 2))
    plasma_pistol_standard:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 3
        Int strenght = 7
        Int penetration = -2
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))
    plasma_pistol_supercharge:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 3
        Int strenght = 8
        Int penetration = -3
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::hazardous),
            weapon_rule(WeaponRuleKind::pistol)
        )
    accursed_weapon:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 5
        Int penetration = -2
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    close_combat_weapon:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 3
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    cultist_autopistol:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 4
        Int strenght = 3
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))
    cultist_bolt_pistol:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 4
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))
    brutal_assault_weapon:
        Int range = 0
        Stat attacks = raw_stat(2)
        Int skill = 4
        Int strenght = 3
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    beat_boss_shoota:
        Int range = 18
        Stat attacks = raw_stat(2)
        Int skill = 4
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::rapid_fire, 1))
    beast_snagga_klaw:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 10
        Int penetration = -2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::anti_monster, 4),
            weapon_rule(WeaponRuleKind::anti_vehichle, 4)
        )
    beastchoppa:
        Int range = 0
        Stat attacks = raw_stat(6)
        Int skill = 2
        Int strenght = 6
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::anti_monster, 4),
            weapon_rule(WeaponRuleKind::anti_vehichle, 4))
    slugga:
        Int range = 12
        Stat attacks = raw_stat(1)
        Int skill = 5
        Int strenght = 4
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))
    thump_gun:
        Int range = 18
        Stat attacks = dice_stat(3)  
        Int skill = 5
        Int strenght = 6
        Int penetration = 0
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::blast))
    choppa:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 3
        Int strenght = 5
        Int penetration = -1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    power_snappa:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 7
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()
    saddlegit_weapons:
        Int range = 9
        Stat attacks = raw_stat(1)
        Int skill = 4
        Int strenght = 3
        Int penetration = 0
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::assault)
        )
    stikka_ranged:
        Int range = 9
        Stat attacks = raw_stat(1)
        Int skill = 5
        Int strenght = 5
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::assault),
            weapon_rule(WeaponRuleKind::anti_monster, 4),
            weapon_rule(WeaponRuleKind::anti_vehichle, 4)
        )
    big_choppa:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 6
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::anti_monster, 4),
            weapon_rule(WeaponRuleKind::anti_vehichle, 4)
        )
    stikka_melee:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 3
        Int strenght = 5
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::anti_monster, 4),
            weapon_rule(WeaponRuleKind::anti_vehichle, 4),
            weapon_rule(WeaponRuleKind::lance)
        )
    squighog_jaws:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 4
        Int strenght = 6
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::extra_attacks)
        )
    vaultswords_behemor:
        Int range = 0
        Stat attacks = raw_stat(6)
        Int skill = 2
        Int strenght = 7
        Int penetration = -2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::precision))
    vaultswords_hurricanus:
        Int range = 0
        Stat attacks = raw_stat(9)
        Int skill = 2
        Int strenght = 5
        Int penetration = -1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::sustained_hit, 1))
    vaultswords_victus:
        Int range = 0
        Stat attacks = raw_stat(5)
        Int skill = 2
        Int strenght = 6
        Int penetration = -3
        Stat damage = raw_stat(3)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::devastating_wounds))
    sentinel_blade_ranged:
        Int range = 12
        Stat attacks = raw_stat(2)
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::assault),
            weapon_rule(WeaponRuleKind::pistol)
        )
    sentinel_blade_melee:
        Int range = 0
        Stat attacks = raw_stat(5)
        Int skill = 2
        Int strenght = 6
        Int penetration = -2
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules()
    castellan_axe_ranged:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::assault))
    guardian_spear_ranged:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::assault))
    castellan_axe_melee:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 2
        Int strenght = 9
        Int penetration = -1
        Stat damage = raw_stat(3)
        WeaponRules rules = weapon_rules()
    guardian_spear_melee:
        Int range = 0
        Stat attacks = raw_stat(5)
        Int skill = 2
        Int strenght = 7
        Int penetration = -2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()
    balistus_grenade_launcher:
        Int range = 18
        Stat attacks = dice_stat(6)  
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::blast))

    guardian_spear_ranged_allarus:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::assault))

    guardian_spear_melee_allarus:
        Int range = 0
        Stat attacks = raw_stat(5)
        Int skill = 2
        Int strenght = 7
        Int penetration = -2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()
    boltstorm_gauntlet:
        Int range = 12
        Stat attacks = raw_stat(3)
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))
    power_fist_zacharial:
        Int range = 0
        Stat attacks = raw_stat(5)
        Int skill = 2
        Int strenght = 8
        Int penetration = -2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()
    relic_chainsword:
        Int range = 0
        Stat attacks = raw_stat(3)
        Int skill = 2
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::extra_attacks)
        )
    bolt_rifle:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::assault),
            weapon_rule(WeaponRuleKind::heavy)
        )
    plasma_incinerator_standard:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 7
        Int penetration = -2
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::assault),
            weapon_rule(WeaponRuleKind::heavy)
        )

    plasma_incinerator_supercharge:
        Int range = 24
        Stat attacks = raw_stat(2)
        Int skill = 3
        Int strenght = 8
        Int penetration = -3
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules(
            weapon_rule(WeaponRuleKind::assault),
            weapon_rule(WeaponRuleKind::hazardous),
            weapon_rule(WeaponRuleKind::heavy)
        )
    heavy_bolt_pistol:
        Int range = 18
        Stat attacks = raw_stat(1)
        Int skill = 3
        Int strenght = 4
        Int penetration = -1
        Stat damage = raw_stat(1)
        WeaponRules rules = weapon_rules(weapon_rule(WeaponRuleKind::pistol))

    master_crafted_power_weapon:
        Int range = 0
        Stat attacks = raw_stat(4)
        Int skill = 3
        Int strenght = 5
        Int penetration = -2
        Stat damage = raw_stat(2)
        WeaponRules rules = weapon_rules()


    fun get_rule_parameter(WeaponRuleKind rule) -> Int:
        let i = 0
        let rules = self.rules()
        while i != rules.size():
            if rules[i].kind == rule: 
                return rules[i].parameter
            i = i + 1
        return 0 

    fun has_rule(WeaponRuleKind rule) -> Bool:
        let i = 0
        let rules = self.rules()
        while i != rules.size():
            if rules[i].kind == rule: 
                return true 
            i = i + 1
        return false

fun write_in_observation_tensor(Weapon obj, Int observer_id, Vector<Float> output, Int index):
    output[index] = (float(obj.value) - (float(max(obj)) / 2.0)) / float(max(obj))
    index = index + 1

fun size_as_observation_tensor(Weapon kind) -> Int:
    return 1

