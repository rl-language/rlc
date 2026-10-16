import bounded_arg
import weapons

enum AbilityKind:
    deep_strike
    oath_of_moment
    veil_of_time
    leader
    unstoppable_valour
    fight_first
    fury_of_the_first
    infiltrators
    lone_operative
    stealth
    shadow_in_the_warp
    neural_disruption
    psychological_saboteur
    feeder_tendrils 
    pouncing_leap
    sacrificial_dagger 
    dark_pacts
    veterans_of_the_long_war
    morgrim
    beast_snagga
    warboss
    beastboss
    refuse_to_yield
    patrol_squads
    bladeguard              
    swords_of_the_chapter  
    shields_of_the_chapter 

    fun equal(AbilityKind other) -> Bool:
        return self.value == other.value

fun write_in_observation_tensor(AbilityKind obj, Int observer_id, Vector<Float> output, Int index):
    output[index] = (float(obj.value) - (float(max(obj)) / 2.0)) / float(max(obj))
    index = index + 1

fun size_as_observation_tensor(AbilityKind kind) -> Int:
    return 1

enum Faction:
    strike_force_octavius
    insidious_infiltrators
    zarkan_daemonkin
    morgrim_butchas
    tristraen_gilded_blade
    vengeful_brethren

    fun equal(Faction other) -> Bool:
        return self.value == other.value

enum Keyword:
    infantry
    imperium
    terminator
    terminator_squad
    tacticus
    infernus_squad
    intercessor_squad
    hellblaster_squad
    character
    captain
    octavius
    psyker
    adeptus_astartes 
    librarian_tantus
    great_devourer
    death_shadow
    vanguard_invader
    neurolictor
    von_ryan_leaper 
    lictor
    tyranids
    chaos_space_marine
    master_of_possession
    aranis_zarkan
    daemon
    possessed
    chaos
    legionaries
    damned
    cultist_mob
    mob
    mounted
    monster_hunters
    beast_snagga
    beastboss
    squighog_boyz
    blade_champion
    tristraen
    custodian_guard
    custodian_wardens
    allarus_custodians
    terminator
    gravis
    master_zacharial
    bladeguard_veteran_squad
    monster # only implemented for veteran instincts 
    vehicle # only implemented for vetern instincts

    fun equal(Keyword other) -> Bool:
        return self.value == other.value

fun write_in_observation_tensor(Keyword obj, Int observer_id, Vector<Float> output, Int index):
    output[index] = (float(obj.value) - (float(max(obj)) / 2.0)) / float(max(obj))
    index = index + 1

fun size_as_observation_tensor(Keyword kind) -> Int:
    return 1

enum Profile:
    captain_octavius:
        Int movement = 5
        Int thoughness = 5
        Int save = 2
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 6
        Int leadership = 6
        Int control = 1
        Float base_size = 50.0
    librarian_tantus:
        Int movement = 5
        Int thoughness = 5
        Int save = 2
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 5
        Int leadership = 6
        Int control = 1
        Float base_size = 40.0
    infernus_squad:
        Int movement = 6
        Int thoughness = 4
        Int save = 3
        Int invuln_save = 10
        Int feel_no_pain = 10
        Int wounds = 2
        Int leadership = 6
        Int control = 1
        Float base_size = 32.0
    terminator_squad:
        Int movement = 5
        Int thoughness = 5
        Int save = 2
        Int invuln_save = 4
        Int wounds = 3
        Int leadership = 6
        Int control = 1
        Float base_size = 40.0
    death_shadow:
        Int movement = 8
        Int thoughness = 5
        Int save = 4
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 7
        Int leadership = 7
        Int control = 1
        Float base_size = 50.0
    lictor:
        Int movement = 8
        Int thoughness = 6
        Int save = 4
        Int invuln_save = 10
        Int wounds = 6
        Int leadership = 7
        Int control = 1
        Float base_size = 50.0
    von_ryan_leaper:
        Int movement = 10
        Int thoughness = 5
        Int save = 4
        Int invuln_save = 6
        Int feel_no_pain = 10
        Int wounds = 3
        Int leadership = 8
        Int control = 1
        Float base_size = 40.0
    aranis_zarkan:
        Int movement = 8
        Int thoughness = 4
        Int save = 3
        Int invuln_save = 5
        Int wounds = 4
        Int leadership = 6
        Int control = 1
        Float base_size = 40.0
    possessed:
        Int movement = 9
        Int thoughness = 6
        Int save = 3
        Int invuln_save = 5
        Int feel_no_pain = 10
        Int wounds = 3
        Int leadership = 6
        Int control = 1
        Float base_size = 40.0
    legionaries:
        Int movement = 6
        Int thoughness = 4
        Int save = 3
        Int invuln_save = 10
        Int wounds = 2
        Int leadership = 6
        Int control = 2
        Float base_size = 32.0
    cultist_mob:
        Int movement = 6
        Int thoughness = 3
        Int save = 6
        Int invuln_save = 10
        Int feel_no_pain = 10
        Int wounds = 1
        Int leadership = 7
        Int control = 1
        Float base_size = 25.0
    beastboss_morgrim:
        Int movement = 6
        Int thoughness = 5
        Int save = 4
        Int invuln_save = 5
        Int feel_no_pain = 5
        Int wounds = 6
        Int leadership = 6
        Int control = 1
        Float base_size = 50.0
    beast_snagga_boy:
        Int movement = 6
        Int thoughness = 5
        Int save = 5
        Int invuln_save = 10
        Int feel_no_pain = 6
        Int wounds = 2
        Int leadership = 7
        Int control = 2
        Float base_size = 32.0
    beast_snagga_nob:
        Int movement = 6
        Int thoughness = 5
        Int save = 5
        Int invuln_save = 10
        Int wounds = 3    
        Int feel_no_pain = 6
        Int leadership = 7
        Int control = 2
        Float base_size = 32.0
    squighog_boyz:
        Int movement = 10
        Int thoughness = 7
        Int save = 4
        Int invuln_save = 10
        Int feel_no_pain = 5
        Int wounds = 3
        Int leadership = 7
        Int control = 2
        Float base_size = 75.0
    nob_on_smasha_squig:
        Int movement = 10
        Int thoughness = 7
        Int save = 4
        Int invuln_save = 10
        Int feel_no_pain = 5
        Int wounds = 4
        Int leadership = 7
        Int control = 2
        Float base_size = 90.0
    tristraen:
        Int movement = 6
        Int thoughness = 6
        Int save = 2
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 6
        Int leadership = 6
        Int control = 2
        Float base_size = 40.0
    custodian_wardens:
        Int movement = 6
        Int thoughness = 6
        Int save = 2
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 3
        Int leadership = 6
        Int control = 2
        Float base_size = 40.0
    allarus_custodians:
        Int movement = 5
        Int thoughness = 7
        Int save = 2
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 4
        Int leadership = 6
        Int control = 2
        Float base_size = 40.0
    custodian_guard:
        Int movement = 6
        Int thoughness = 6
        Int save = 2
        Int invuln_save = 4
        Int feel_no_pain = 10
        Int wounds = 4
        Int leadership = 6
        Int control = 2
        Float base_size = 40.0
    master_zacharial:
        Int movement = 5
        Int thoughness = 6
        Int feel_no_pain = 10
        Int save = 3
        Int invuln_save = 4
        Int wounds = 6
        Int leadership = 6
        Int control = 1
        Float base_size = 40.0
    intercessor_squad:
        Int movement = 6
        Int thoughness = 4
        Int save = 3
        Int invuln_save = 10
        Int feel_no_pain = 10
        Int wounds = 2
        Int leadership = 6
        Int control = 2
        Float base_size = 32.0
    hellblaster_squad:
        Int movement = 6
        Int thoughness = 4
        Int save = 3
        Int invuln_save = 10
        Int feel_no_pain = 10
        Int wounds = 2
        Int leadership = 6
        Int control = 1
        Float base_size = 32.0
    bladeguard_veteran_squad:
        Int movement = 6
        Int thoughness = 4
        Int save = 3
        Int invuln_save = 4
        Int wounds = 3
        Int leadership = 6
        Int control = 1
        Float base_size = 40.0


    fun equal(Profile other) -> Bool:
        return self.value == other.value

fun write_in_observation_tensor(Profile obj, Int observer_id, Vector<Float> output, Int index):
    output[index] = (float(obj.value) - (float(max(obj)) / 2.0)) / float(max(obj))
    index = index + 1

fun size_as_observation_tensor(Profile kind) -> Int:
    return 1

enum Stratagem:
    reroll:
        Int cost = 1
    overwatch:
        Int cost = 1 
    rapid_ingress:
        Int cost = 1
    insane_bravery:
        Int cost = 1
    veteran_instincts:
        Int cost = 1
    gene_wrought_resiliance:
        Int cost = 1 
    duty_and_honour:
        Int cost = 1 
    heroic_intervention:
        Int cost = 2
    swift_kill:
        Int cost = 1
    pheromone_trace:
        Int cost = 1
    predators_not_prey:
        Int cost = 1
    vindictive_strategy:
        Int cost = 1
    violent_unbidding:
        Int cost = 1
    demonic_fervour:
        Int cost = 1
    tough_as_squig_hide:
        Int cost = 1
    bestial_bellow:
        Int cost = 1
    overawing_magnificence:
        Int cost = 1
    the_gilded_spear:
        Int cost = 1
    relic_munitions:
        Int cost = 1
    unyielding:
        Int cost = 1


    fun equal(Stratagem other) -> Bool:
        return self.value == other.value
