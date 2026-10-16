import bounded_arg
import stats
import vector2d
import machine_learning
import range

using Wounds = LinearlyDistributedInt<0, 10>
const MAX_WEAPONS = 4
using WeaponsVector = BoundedVector<Weapon, MAX_WEAPONS>
using Abilities = BoundedVector<AbilityKind, 4>
using Keywords = BoundedVector<Keyword, 7>

# some rules specify that models are not to be removed after being destroyed until some other event happens, for example fight on death. We keep track of this special statuses with this enum
enum ModelState:
    normal
    fight_on_death

    fun equal(ModelState other) -> Bool:
        return self.value == other.value

cls Model:
    BoardPosition position # required
    Profile profile
    Wounds suffered_wounds
    WeaponsVector weapons
    Abilities abilities 
    Keywords keywords 
    ModelState state

    fun base_size_in_inches() -> Float:
        return self.profile.base_size() / 25.4

    fun is_below_half_wounds() -> Bool:
        return self.suffered_wounds * 2 > self.profile.wounds()

    fun has_keyword(Keyword keyword) -> Bool:
        let i = 0
        let rules = self.keywords
        while i != rules.size():
            if rules[i] == keyword: 
                return true 
            i = i + 1
        return false

    fun has_weapon_rule(WeaponRuleKind rule) -> Bool:
        let i = 0
        while i != self.weapons.size():
            if self.weapons[i].has_rule(rule):
                return true 
            i = i + 1
        return false

    fun has_ability(AbilityKind kind) -> Bool:
        let i = 0
        let rules = self.abilities
        while i != rules.size():
            if rules[i] == kind: 
                return true 
            i = i + 1
        return false

    fun consolidate_torward(Unit other):
        let consolidation = other.get_shortest_vector_to(self) * -1.0
    
        if consolidation.length() > 2.0:
            consolidation = (consolidation.as_float_vector().normalized() * 2.0).as_int_vector()
        self.position = self.position + consolidation

    fun is_character() -> Bool:
        return self.has_keyword(Keyword::character)

    fun distance(Model model) -> Float:
        return (model.position.as_vector() - self.position.as_vector()).length() - ((model.base_size_in_inches() + self.base_size_in_inches()) /2.0)

    fun wounds_left() -> Int:
        return self.profile.wounds() - self.suffered_wounds.value

const MAX_UNIT_MODELS = 12
const MAX_UNIT_MODELS_PLUS1 = 13
using ModelVector = BoundedVector<Model, MAX_UNIT_MODELS>

cls ModelID: # required
    BInt<0, MAX_UNIT_MODELS> id

    fun get() -> Int:
        return self.id.value

    fun assign(Int other):
        self.id = other

using ModelIterator = LinearlyDistributedInt<0, MAX_UNIT_MODELS_PLUS1>

const MAX_UNIT_COUNT = 10
const MAX_UNIT_COUNT_PLUS1 = 11
cls UnitID: # required
    BInt<0, MAX_UNIT_COUNT> id

    fun get() -> Int:
        return self.id.value

    fun assign(Int value):
        self.id = value

using UnitIterator = BInt<0, MAX_UNIT_COUNT_PLUS1> 

fun unit_id(Int id) -> UnitID:
    let to_return :  UnitID
    to_return = id
    return to_return

cls PhaseModifiers:
    Bool greater_strenght_wound_protection 

cls Unit:
    ModelVector models # required
    Bool owned_by_player1 # required
    Hidden<String> name # required
    Bool has_moved
    Bool has_run 
    Bool has_fought
    Bool has_shoot
    Bool battle_socked
    Bool has_charged
    Bool can_shoot
    Bool can_charge
    ModelID starting_strenght
    PhaseModifiers phase_modifiers

    fun has_greater_strenght_wound_protection() -> Bool:
        return self.phase_modifiers.greater_strenght_wound_protection 

    fun clear_phase_modifiers():
        let mod : PhaseModifiers
        self.phase_modifiers = mod

    fun is_lone_operative() -> Bool:
        return self.has_ability(AbilityKind::lone_operative)

    fun remove_figth_on_death_models():
        let i = self.models.size() - 1
        while i > 0:
            if self.models[i].state == ModelState::fight_on_death:
                self.models.erase(i)
            i = i - 1

    fun can_reroll_charge() -> Bool:
        return self.has_ability(AbilityKind::unstoppable_valour)

    fun owner_id() -> Int:
        return int(self.owned_by_player1)
    
    fun get_temporary_weapon_rules() -> BoundedVector<WeaponRule, 1>:
        let to_return : BoundedVector<WeaponRule, 1>
        if self.has_ability(AbilityKind::veil_of_time)  and self.models.size() != 1:
            to_return.append(weapon_rule(WeaponRuleKind::sustained_hit, 1))
        return to_return

    fun empty() -> Bool:
        return self.models.empty()

    fun consolidate_torward(Unit other):
        let i = 0
        while i != self.models.size():
            self.models[i].consolidate_torward(other)
            i = i + 1

    fun attach_to(Unit other):
        assert(self.models.size() == 1, "only units with one model can be attached")
        other.models.append(self.models[0])
        self.models.erase(0)

    fun get_leadership() -> Int:
        if self.models.empty():
            return 0
        let best_leadership = self.models[0].profile.leadership()
        let i = 1
        while i != self.models.size():
            best_leadership = max(best_leadership, self.models[i].profile.leadership())
            i = i + 1

        return best_leadership
        
    fun has_temporary_wepon_rule(WeaponRuleKind rule) -> Bool: 
        let temp_rules = self.get_temporary_weapon_rules()
        let i = 0
        while i != temp_rules.size():
            if temp_rules[i].kind == rule:
                return true
            i = i + 1
        return false

    fun is_below_half_strenght() -> Bool:
        if self.starting_strenght.get() == 1:
            return self.models[0].is_below_half_wounds()
        
        return self.starting_strenght.get() / 2 > self.models.size()

    fun get_temporary_weapon_parameter(WeaponRuleKind rule) -> Int: 
        let temp_rules = self.get_temporary_weapon_rules()
        let i = 0
        while i != temp_rules.size():
            if temp_rules[i].kind == rule:
                return temp_rules[i].parameter
            i = i + 1
        return 0

    fun max_weapon_parameter(Weapon w, WeaponRuleKind kind) -> Int:
        return max(w.get_rule_parameter(kind), self.get_temporary_weapon_parameter(kind)) 

    fun all_have_ability(AbilityKind kind) -> Bool:
        let i = 0
        while i != self.models.size():
            if !self.models[i].has_ability(kind):
                return false
            i = i + 1
        return true

    fun all_have_keyword(Keyword kind) -> Bool:
        let i = 0
        while i != self.models.size():
            if !self.models[i].has_keyword(kind):
                return false
            i = i + 1
        return true

    fun has_keyword(Keyword kind) -> Bool:
        let i = 0
        while i != self.models.size():
            if self.models[i].has_keyword(kind):
                return true
            i = i + 1
        return false

    fun has_ability(AbilityKind kind) -> Bool:
        let i = 0
        while i != self.models.size():
            if self.models[i].has_ability(kind):
                return true 
            i = i + 1
        return false

    fun get_unit_toughtness() -> Int:
        if self.models.size() == 0:
            return 0
        let max_toughness = self.models[0].profile.thoughness()
        let i = 1
        while i != self.models.size():
            let max_toughness = max(self.models[i].profile.thoughness(), max_toughness)
            i = i + 1
        return max_toughness 

    fun translate(Int x, Int y):
        let v : Vector2D
        v.x = x
        v.y = y
        self.translate(v)


    fun move_to(BoardPosition new_position):
        if self.models.size() == 0:
            return
        let v = new_position.as_vector() - self.models[0].position.as_vector()
        self.translate(v)

    fun translate(Vector2D v):
        let i = 0
        while i != self.models.size():
            ref model = self.models[i]
            model.position = model.position + v
            i = i + 1

    fun distance(BoardPosition position) -> Float:
        return self.distance(position.as_vector()) 

    fun distance(Unit unit) -> Float:
        if unit.models.size() == 0:
            return 100.0
        if self.models.size() == 0:
            return 100.0
        return self.distance(unit.models[0]) 

    fun distance(Model model) -> Float:
        assert(self.models.size() != 0, "calculanting distance to empty unit")
        let shortest = self.models[0].distance(model)
        let i = 1
        while i != self.models.size():
            if shortest > self.models[i].distance(model):
                shortest = self.models[i].distance(model)
            i = i + 1
        return shortest 

    fun get_shortest_vector_to(Unit other) -> Vector2D:
        let their_nearest_model = 0
        let our_nearest_model = 0
        let shortest_lenght = 100.0
        let i = 1
        while i != self.models.size():
            let j = 0
            while j != other.models.size():
                if shortest_lenght > self[i].distance(other[j]):
                    shortest_lenght = self[i].distance(other[j])
                    their_nearest_model = j
                    our_nearest_model = i
                j = j + 1
            i = i + 1
        return other.models[their_nearest_model].position.as_vector() - self.models[our_nearest_model].position.as_vector()

    fun get_shortest_vector_to(Model model) -> Vector2D:
        let nearest_model = self.get_nearest_model_index(model)
        return model.position.as_vector() - self.models[nearest_model].position.as_vector()

    fun get_nearest_model_index(Model model) -> Int:
        assert(self.models.size() != 0, "calculanting distance to empty unit")
        let nearest_model = 0 
        let i = 1
        while i != self.models.size():
            if self.models[nearest_model].distance(model) > self.models[i].distance(model):
                nearest_model = i
            i = i + 1
        return nearest_model

    fun distance(Vector2D position) -> Float:
        assert(self.models.size() != 0, "calculanting distance to empty unit")
        let nearest = self.models[0].position.as_vector()
        let i = 1
        while i != self.models.size():
            if (nearest - position).length() > (self.models[i].position.as_vector() - position).length():
                nearest = self.models[i].position.as_vector()
            i = i + 1
        return (nearest - position).length() 

    fun get(Int model_id) -> ref Model:
        return self.models[model_id]

    fun get(ModelID model_id) -> ref Model:
        return self.models[model_id.get()]

    fun arrange():
        if self.models.size() == 1:
            return
        let i = 1
        while i != self.models.size():
            ref model = self.models[i]
            model.position = self.models[i-1].position
            model.position.x = model.position.x + int(1.0 + model.base_size_in_inches())
            i = i + 1

    fun damage(Int target_model_id, Int damage) -> Bool:
        ref target_model = self.models[target_model_id]
        target_model.suffered_wounds = target_model.suffered_wounds + damage 
        if target_model.suffered_wounds >= target_model.profile.wounds(): 
            self.models.erase(target_model_id)
            return true
        return false

    fun damage(Int target_model_id, Int damage, Bool fight_on_death) -> Bool:
        ref target_model = self.models[target_model_id]
        target_model.suffered_wounds = target_model.suffered_wounds + damage 
        if target_model.suffered_wounds >= target_model.profile.wounds(): 
            if fight_on_death:
                target_model.state = ModelState::fight_on_death
            else:
                self.models.erase(target_model_id)
            return true
        return false

    fun damage(ModelID target_model_id) -> Bool:
        return self.damage(target_model_id.get(), 1)

    fun deal_mortal_wound_damage(Int wounds):
        for i in range(wounds):
            self.damage(0, 1)
            if self.models.size() == 0:
                return
        


using UnitVector = BoundedVector<Unit, MAX_UNIT_COUNT>


fun make_tantus() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::librarian_tantus
    model.abilities.append(AbilityKind::deep_strike)
    model.abilities.append(AbilityKind::leader)
    model.abilities.append(AbilityKind::oath_of_moment)
    model.abilities.append(AbilityKind::veil_of_time)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::character)
    model.keywords.append(Keyword::imperium)
    model.keywords.append(Keyword::terminator)
    model.keywords.append(Keyword::psyker)
    model.keywords.append(Keyword::octavius)
    model.keywords.append(Keyword::librarian_tantus)
    model.weapons.append(Weapon::librarian_storm_bolter)
    model.weapons.append(Weapon::smite_witchfire)
    model.weapons.append(Weapon::smite_focused_witchfire)
    model.weapons.append(Weapon::force_weapon)
    profile.name = "librarian tantus"s
    profile.models.append(model)
    return profile

fun make_infernus_squad() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::infernus_squad
    model.abilities.append(AbilityKind::oath_of_moment)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::imperium)
    model.keywords.append(Keyword::tacticus)
    model.keywords.append(Keyword::infernus_squad)
    model.weapons.append(Weapon::bolt_pistol)
    model.weapons.append(Weapon::pyreblaster)
    model.weapons.append(Weapon::close_combact_weapon)
    profile.name = "infernus squad"s
    profile.models.append(model)
    profile.models.append(model)
    profile.models.append(model)
    profile.models.append(model)
    profile.models.append(model)
    return profile

fun make_terminator_squad() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::terminator_squad
    model.abilities.append(AbilityKind::oath_of_moment)
    model.abilities.append(AbilityKind::fury_of_the_first)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::imperium)
    model.keywords.append(Keyword::terminator)
    model.keywords.append(Keyword::terminator_squad)
    model.weapons.append(Weapon::terminator_storm_bolter)
    model.weapons.append(Weapon::terminator_power_fist)
    profile.name = "terminator squad"s
    profile.models.append(model)
    profile.models.append(model)
    profile.models.append(model)

    model.weapons.clear()
    model.weapons.append(Weapon::terminator_storm_bolter)
    model.weapons.append(Weapon::terminator_power_weapon)
    profile.models.append(model)

    model.weapons.clear()
    model.weapons.append(Weapon::terminator_power_fist)
    model.weapons.append(Weapon::assault_cannon)
    profile.models.append(model)

    return profile

fun make_octavius() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::captain_octavius
    model.abilities.append(AbilityKind::deep_strike)
    model.abilities.append(AbilityKind::leader)
    model.abilities.append(AbilityKind::oath_of_moment)
    model.abilities.append(AbilityKind::unstoppable_valour)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::character)
    model.keywords.append(Keyword::imperium)
    model.keywords.append(Keyword::terminator)
    model.keywords.append(Keyword::captain)
    model.keywords.append(Keyword::octavius)
    model.keywords.append(Keyword::adeptus_astartes)
    model.weapons.append(Weapon::captain_storm_bolter)
    model.weapons.append(Weapon::relic_weapon)
    profile.name = "captain octavious"s
    profile.models.append(model)
    return profile

fun make_death_shadow() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::death_shadow
    model.abilities.append(AbilityKind::infiltrators)
    model.abilities.append(AbilityKind::lone_operative)
    model.abilities.append(AbilityKind::stealth)
    model.abilities.append(AbilityKind::shadow_in_the_warp)
    model.abilities.append(AbilityKind::neural_disruption)
    model.abilities.append(AbilityKind::psychological_saboteur)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::great_devourer)
    model.keywords.append(Keyword::death_shadow)
    model.keywords.append(Keyword::vanguard_invader)
    model.keywords.append(Keyword::neurolictor)
    model.keywords.append(Keyword::tyranids)
    model.weapons.append(Weapon::death_shadow_claws_and_talons)
    profile.name = "death's shadow"s
    profile.models.append(model)
    return profile

fun make_lictor() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::lictor
    model.abilities.append(AbilityKind::infiltrators)
    model.abilities.append(AbilityKind::lone_operative)
    model.abilities.append(AbilityKind::stealth)
    model.abilities.append(AbilityKind::fight_first)
    model.abilities.append(AbilityKind::feeder_tendrils)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::great_devourer)
    model.keywords.append(Keyword::lictor)
    model.keywords.append(Keyword::vanguard_invader)
    model.keywords.append(Keyword::tyranids)
    model.weapons.append(Weapon::lictor_claws_and_talons)
    profile.name = "lictor"s
    profile.models.append(model)
    return profile

fun make_von_ryan_leaper() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::von_ryan_leaper
    model.abilities.append(AbilityKind::infiltrators)
    model.abilities.append(AbilityKind::stealth)
    model.abilities.append(AbilityKind::fight_first)
    model.abilities.append(AbilityKind::pouncing_leap)
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::von_ryan_leaper)
    model.keywords.append(Keyword::great_devourer)
    model.keywords.append(Keyword::vanguard_invader)
    model.keywords.append(Keyword::tyranids)
    model.weapons.append(Weapon::leapers_talon)
    profile.name = "von ryan's leaper"s
    profile.models.append(model)
    profile.models.append(model)
    profile.models.append(model)
    return profile

fun make_aranis_zarkan() -> Unit:
    let profile : Unit 
    let model : Model
    model.profile = Profile::aranis_zarkan
    model.abilities.append(AbilityKind::dark_pacts)
    model.abilities.append(AbilityKind::leader)

    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::character)
    model.keywords.append(Keyword::psyker)
    model.keywords.append(Keyword::chaos)
    model.keywords.append(Keyword::master_of_possession)
    model.keywords.append(Keyword::aranis_zarkan)
    model.weapons.append(Weapon::rite_of_possession)
    model.weapons.append(Weapon::rite_of_possession_focused)
    model.weapons.append(Weapon::staff_of_possession)
    profile.name = "aranis zarkan"s
    profile.models.append(model)
    return profile

fun make_possessed() -> Unit:
    let profile : Unit 
    let model : Model
    
    model.profile = Profile::possessed
    
    model.abilities.append(AbilityKind::dark_pacts)
    
    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::chaos)
    model.keywords.append(Keyword::daemon)      
    model.keywords.append(Keyword::possessed)   

    model.weapons.append(Weapon::hideous_mutations)
    
    profile.name = "Possessed"s
    for i in range(5):
        profile.models.append(model)
    return profile

fun make_cultist_mob() -> Unit:
    let profile : Unit
    let champion : Model
    champion.profile = Profile::cultist_mob
    champion.abilities.append(AbilityKind::dark_pacts)
    champion.keywords.append(Keyword::infantry)
    champion.keywords.append(Keyword::chaos)
    champion.keywords.append(Keyword::damned)
    champion.keywords.append(Keyword::cultist_mob) 
    champion.weapons.append(Weapon::cultist_bolt_pistol)
    champion.weapons.append(Weapon::brutal_assault_weapon)

    let i = 0
    while i != 9:
        let cultist : Model
        cultist.profile = Profile::cultist_mob
        cultist.abilities.append(AbilityKind::dark_pacts)
        cultist.keywords.append(Keyword::infantry)
        cultist.keywords.append(Keyword::chaos)
        cultist.keywords.append(Keyword::damned)
        cultist.keywords.append(Keyword::cultist_mob)
        cultist.weapons.append(Weapon::cultist_autopistol)
        cultist.weapons.append(Weapon::brutal_assault_weapon)
        profile.models.append(cultist)
        i = i + 1

    profile.models.append(champion)

    profile.name = "Cultist Mob"s
    return profile

fun make_legionaries() -> Unit:
    let profile : Unit
    profile.name = "Legionaries"s

    # Prepare a single Model object with shared stats, abilities, and keywords
    let model : Model
    model.profile = Profile::legionaries
    model.abilities.append(AbilityKind::dark_pacts)
    # model.abilities.append(AbilityKind::veterans_of_the_long_war)  # if you’ve defined it

    model.keywords.append(Keyword::infantry)
    model.keywords.append(Keyword::chaos)
    model.keywords.append(Keyword::damned)
    model.keywords.append(Keyword::legionaries)
    # model.keywords.append(Keyword::battleline)  # if you’ve defined it

    # 1) ASPIRING CHAMPION
    model.weapons.clear()
    model.weapons.append(Weapon::plasma_pistol_standard)
    model.weapons.append(Weapon::plasma_pistol_supercharge)
    model.weapons.append(Weapon::accursed_weapon)
    profile.models.append(model)  # This copies champion’s state into the unit

    # 2) LEGIONARY with Meltagun
    model.weapons.clear()
    model.weapons.append(Weapon::bolt_pistol)
    model.weapons.append(Weapon::meltagun)
    model.weapons.append(Weapon::close_combat_weapon)
    profile.models.append(model)  # Copies meltagun loadout

    # 3) LEGIONARY with Heavy Bolter
    model.weapons.clear()
    model.weapons.append(Weapon::bolt_pistol)
    model.weapons.append(Weapon::heavy_bolter)
    model.weapons.append(Weapon::close_combat_weapon)
    profile.models.append(model)  # Copies heavy bolter loadout

    # 4) 7 LEGIONARIES with Boltgun
    for i in range(7):
        model.weapons.clear()
        model.weapons.append(Weapon::bolt_pistol)
        model.weapons.append(Weapon::boltgun)
        model.weapons.append(Weapon::close_combat_weapon)
        profile.models.append(model)  # Copies boltgun loadout each time

    return profile

fun make_beastboss_morgrim() -> Unit:
    let boss_unit : Unit
    let boss_model : Model
    
    # Use the pre-defined profile for Beastboss Morgrim
    boss_model.profile = Profile::beastboss_morgrim
    boss_model.abilities.append(AbilityKind::leader)
    boss_model.abilities.append(AbilityKind::beast_snagga)
    boss_model.abilities.append(AbilityKind::warboss)
    boss_model.abilities.append(AbilityKind::beastboss)
    boss_model.abilities.append(AbilityKind::morgrim)
    
    boss_model.keywords.append(Keyword::infantry)
    boss_model.keywords.append(Keyword::character)
    boss_model.keywords.append(Keyword::beastboss)

    boss_model.weapons.append(Weapon::beat_boss_shoota)

    boss_model.weapons.append(Weapon::beast_snagga_klaw)

    boss_model.weapons.append(Weapon::beastchoppa)

    boss_unit.models.append(boss_model)
    boss_unit.name = "Beastboss Morgrim"s
    return boss_unit

fun make_beast_snagga_boyz() -> Unit:
    let unit : Unit
    unit.name = "Beast Snagga Boyz"s
    let nob_model : Model
    nob_model.profile = Profile::beast_snagga_nob
    nob_model.abilities.append(AbilityKind::beast_snagga)
    nob_model.keywords.append(Keyword::infantry)
    nob_model.keywords.append(Keyword::mob)
    nob_model.keywords.append(Keyword::monster_hunters)
    nob_model.weapons.append(Weapon::slugga)
    nob_model.weapons.append(Weapon::power_snappa)
    unit.models.append(nob_model)
    let thumpgun_boy : Model
    thumpgun_boy.profile = Profile::beast_snagga_boy
    thumpgun_boy.abilities.append(AbilityKind::beast_snagga)
    thumpgun_boy.keywords.append(Keyword::infantry)
    thumpgun_boy.keywords.append(Keyword::mob)
    thumpgun_boy.keywords.append(Keyword::monster_hunters)

    thumpgun_boy.weapons.append(Weapon::thump_gun)
    thumpgun_boy.weapons.append(Weapon::close_combat_weapon)

    unit.models.append(thumpgun_boy)

    let boy : Model
    boy.profile = Profile::beast_snagga_boy
    boy.abilities.append(AbilityKind::beast_snagga)
    boy.keywords.append(Keyword::infantry)
    boy.keywords.append(Keyword::mob)
    boy.keywords.append(Keyword::monster_hunters)

    boy.weapons.append(Weapon::slugga)
    boy.weapons.append(Weapon::choppa)

    for i in range(8):
        unit.models.append(boy)

    return unit

fun make_squighog_boyz() -> Unit:
    let unit : Unit
    unit.name = "Squighog Boyz"s

    let nob_model : Model
    nob_model.profile = Profile::nob_on_smasha_squig
    nob_model.abilities.append(AbilityKind::beast_snagga)
    nob_model.keywords.append(Keyword::mounted)
    nob_model.keywords.append(Keyword::beast_snagga)
    nob_model.keywords.append(Keyword::squighog_boyz)
    
    # Weapons: slugga, big choppa, squighog_jaws
    nob_model.weapons.append(Weapon::slugga)
    nob_model.weapons.append(Weapon::big_choppa)
    nob_model.weapons.append(Weapon::squighog_jaws)
    
    unit.models.append(nob_model)

    # 2) 3 standard Squighog Boyz
    for i in range(3):
        let hog_boy : Model
        hog_boy.profile = Profile::squighog_boyz
        hog_boy.abilities.append(AbilityKind::beast_snagga)
        hog_boy.keywords.append(Keyword::mounted)
        hog_boy.keywords.append(Keyword::beast_snagga)
        hog_boy.keywords.append(Keyword::squighog_boyz)

        # Weapons: saddlegit_weapons, stikka_ranged, squighog_jaws
        hog_boy.weapons.append(Weapon::saddlegit_weapons)
        hog_boy.weapons.append(Weapon::stikka_ranged)
        hog_boy.weapons.append(Weapon::squighog_jaws)

        unit.models.append(hog_boy)

    return unit

fun make_tristrean() -> Unit:
    let tristraen_unit : Unit

    # Create the single model for Tristraen
    let tristraen_model : Model
    tristraen_model.profile = Profile::tristraen

    # Abilities (Martial Ka’tah is not yet defined as an AbilityKind, so we omit it here)
    # Add known abilities if you wish, or omit if they are not enumerated:
    # tristraen_model.abilities.append(AbilityKind::some_ability) 

    # Keywords
    tristraen_model.keywords.append(Keyword::infantry)
    tristraen_model.keywords.append(Keyword::character)
    tristraen_model.keywords.append(Keyword::imperium)
    tristraen_model.keywords.append(Keyword::blade_champion)
    tristraen_model.keywords.append(Keyword::tristraen)
    # (No enumerated keyword for “adeptus_custodes” currently, so we skip it.)

    # Melee Weapons
    tristraen_model.weapons.append(Weapon::vaultswords_behemor)
    tristraen_model.weapons.append(Weapon::vaultswords_hurricanus)
    tristraen_model.weapons.append(Weapon::vaultswords_victus)

    # Finish building the unit
    tristraen_unit.name = "Tristraen"s
    tristraen_unit.models.append(tristraen_model)

    return tristraen_unit

fun make_custodian_guard() -> Unit:
    let guard_unit : Unit
    guard_unit.name = "Custodian Guard"s

    # Each Guard model is Profile::custodian_guard (T=6, W=3, etc.).
    # Praesidium Shield adds +1 to Wounds (to 4), but there is no direct code mechanism to
    # modify an enum-based Profile. In a complete implementation, you might add a new Profile
    # or some special logic for the shield. For now, we’ll just note it here.

    let guard_model : Model
    guard_model.profile = Profile::custodian_guard

    # Add relevant Keywords (INFANTRY, IMPERIUM, CUSTODIAN GUARD).
    guard_model.keywords.append(Keyword::infantry)
    guard_model.keywords.append(Keyword::imperium)
    # We do not have a dedicated enum entry for “CUSTODIAN GUARD,” but we can at least note it:
    guard_model.keywords.append(Keyword::custodian_guard)

    # Sentinel Blade (ranged + melee):
    guard_model.weapons.append(Weapon::sentinel_blade_ranged)
    guard_model.weapons.append(Weapon::sentinel_blade_melee)

    # Make 3 models:
    for i in range(3):
        guard_unit.models.append(guard_model)

    return guard_unit

fun make_custodian_wardens() -> Unit:
    let wardens_unit : Unit
    wardens_unit.name = "Custodian Wardens"s

    # Profile: custodian_wardens (M=6, T=6, Sv=2+, W=3, Ld=6, OC=2, Inv=4+)

    # 1) WARDEN with Castellan Axe
    let warden_with_axe : Model
    warden_with_axe.profile = Profile::custodian_wardens
    warden_with_axe.keywords.append(Keyword::infantry)
    warden_with_axe.keywords.append(Keyword::imperium)
    warden_with_axe.keywords.append(Keyword::custodian_wardens)
    warden_with_axe.weapons.append(Weapon::castellan_axe_ranged)
    warden_with_axe.weapons.append(Weapon::castellan_axe_melee)

    # 2) WARDEN with Guardian Spear
    let warden_with_spear : Model
    warden_with_spear.profile = Profile::custodian_wardens
    warden_with_spear.keywords.append(Keyword::infantry)
    warden_with_spear.keywords.append(Keyword::imperium)
    warden_with_spear.keywords.append(Keyword::custodian_wardens)
    warden_with_spear.weapons.append(Weapon::guardian_spear_ranged)
    warden_with_spear.weapons.append(Weapon::guardian_spear_melee)

    # Add them to the unit: 1 with axe, 2 with spear
    wardens_unit.models.append(warden_with_axe)
    wardens_unit.models.append(warden_with_spear)
    wardens_unit.models.append(warden_with_spear)

    return wardens_unit

fun make_allarus_custodians() -> Unit:
    let allarus_unit : Unit
    allarus_unit.name = "Allarus Custodians"s

    # Each Allarus Custodian model:
    let allarus_model : Model
    allarus_model.profile = Profile::allarus_custodians
    allarus_model.keywords.append(Keyword::infantry)
    allarus_model.keywords.append(Keyword::terminator)
    allarus_model.keywords.append(Keyword::imperium)
    allarus_model.keywords.append(Keyword::allarus_custodians)

    # Weapons: balistus grenade launcher + guardian spear (both ranged and melee)
    allarus_model.weapons.append(Weapon::balistus_grenade_launcher)
    allarus_model.weapons.append(Weapon::guardian_spear_ranged_allarus)
    allarus_model.weapons.append(Weapon::guardian_spear_melee_allarus)

    # Add two models
    allarus_unit.models.append(allarus_model)
    allarus_unit.models.append(allarus_model)

    return allarus_unit


fun make_octavious_strike_force(UnitVector out, Bool owner) -> Faction:
    out.append(make_octavius())
    out.back().owned_by_player1 = owner
    out.append(make_tantus())
    out.back().owned_by_player1 = owner
    out.append(make_infernus_squad())
    out.back().owned_by_player1 = owner
    out.append(make_terminator_squad())
    out.back().owned_by_player1 = owner
    return Faction::strike_force_octavius

fun make_insidious_infiltrators(UnitVector out, Bool owner) -> Faction:
    out.append(make_death_shadow())
    out.back().owned_by_player1 = owner
    out.append(make_lictor())
    out.back().owned_by_player1 = owner
    for i in range(3):
        out.append(make_von_ryan_leaper())
        out.back().owned_by_player1 = owner
    return Faction::insidious_infiltrators

fun make_zarkan_deamonkin(UnitVector out, Bool owner) -> Faction:
    out.append(make_aranis_zarkan())
    out.back().owned_by_player1 = owner
    out.append(make_possessed())
    out.back().owned_by_player1 = owner
    out.append(make_cultist_mob())
    out.back().owned_by_player1 = owner
    out.append(make_legionaries())
    out.back().owned_by_player1 = owner
    return Faction::insidious_infiltrators

fun make_morgrim_butchas(UnitVector out, Bool owner) -> Faction:
    out.append(make_beastboss_morgrim())
    out.back().owned_by_player1 = owner
    out.append(make_squighog_boyz())
    out.back().owned_by_player1 = owner
    out.append(make_beast_snagga_boyz())
    out.back().owned_by_player1 = owner
    out.append(make_beast_snagga_boyz())
    out.back().owned_by_player1 = owner
    return Faction::insidious_infiltrators

fun make_tristrean_gilded_blade(UnitVector out, Bool owner) -> Faction:
    # 1) Tristraen
    out.append(make_tristrean())
    out.back().owned_by_player1 = owner

    # 2) Custodian Guard (3 models)
    out.append(make_custodian_guard())
    out.back().owned_by_player1 = owner

    # 3) Custodian Wardens (3 models)
    out.append(make_custodian_wardens())
    out.back().owned_by_player1 = owner

    # 4) Allarus Custodians (2 models)
    out.append(make_allarus_custodians())
    out.back().owned_by_player1 = owner

    return Faction::tristraen_gilded_blade

fun make_master_zacharial() -> Unit:
    let zach_unit : Unit
    
    let zach_model : Model
    zach_model.profile = Profile::master_zacharial

    # Abilities
    zach_model.abilities.append(AbilityKind::oath_of_moment)
    zach_model.abilities.append(AbilityKind::refuse_to_yield)

    # Keywords
    zach_model.keywords.append(Keyword::infantry)
    zach_model.keywords.append(Keyword::character)
    zach_model.keywords.append(Keyword::imperium)
    zach_model.keywords.append(Keyword::gravis)
    zach_model.keywords.append(Keyword::captain)
    zach_model.keywords.append(Keyword::master_zacharial)
    # If you wish to note his Chapter, you can also add:
    # zach_model.keywords.append(Keyword::adeptus_astartes)
    # (No “dark_angels” keyword was enumerated, so we skip it.)

    # Weapons
    zach_model.weapons.append(Weapon::boltstorm_gauntlet)
    zach_model.weapons.append(Weapon::power_fist_zacharial)
    zach_model.weapons.append(Weapon::relic_chainsword)

    zach_unit.name = "Master Zacharial"s
    zach_unit.models.append(zach_model)

    return zach_unit


fun make_intercessor_squad() -> Unit:
    let squad : Unit
    squad.name = "Intercessor Squad"s

    let base_model : Model
    base_model.profile = Profile::intercessor_squad

    # Abilities
    base_model.abilities.append(AbilityKind::oath_of_moment)
    base_model.abilities.append(AbilityKind::patrol_squads)

    # Keywords
    base_model.keywords.append(Keyword::infantry)
    base_model.keywords.append(Keyword::imperium)
    base_model.keywords.append(Keyword::tacticus)
    base_model.keywords.append(Keyword::intercessor_squad)
    # If you wish to add “BATTLELINE” or “DARK_ANGELS,” you would first need to define them
    # in the enum Keyword, similar to:
    #
    #   battleline
    #   dark_angels
    #
    # Then you could append them here.

    # Weapons
    base_model.weapons.append(Weapon::bolt_pistol)
    base_model.weapons.append(Weapon::bolt_rifle)
    base_model.weapons.append(Weapon::close_combat_weapon)

    # Create 10 identical models
    for i in range(10):
        squad.models.append(base_model)

    return squad

fun make_hellblaster_squad() -> Unit:
    let squad : Unit
    squad.name = "Hellblaster Squad"s

    let base_model : Model
    base_model.profile = Profile::hellblaster_squad

    # Abilities
    base_model.abilities.append(AbilityKind::oath_of_moment)

    # Keywords
    base_model.keywords.append(Keyword::infantry)
    base_model.keywords.append(Keyword::imperium)
    base_model.keywords.append(Keyword::tacticus)
    base_model.keywords.append(Keyword::hellblaster_squad)
    # If you wish to add “ADEPTUS_ASTARTES” or “DARK_ANGELS,” define them similarly in enum Keyword first, then append.

    # Weapons
    base_model.weapons.append(Weapon::bolt_pistol)
    base_model.weapons.append(Weapon::plasma_incinerator_standard)
    base_model.weapons.append(Weapon::plasma_incinerator_supercharge)
    base_model.weapons.append(Weapon::close_combat_weapon)

    # 5 identical models
    for i in range(5):
        squad.models.append(base_model)

    return squad

fun make_bladeguard_veteran_squad() -> Unit:
    let squad : Unit
    squad.name = "Bladeguard Veteran Squad"s

    # Prepare a single base Model
    let base_model : Model
    base_model.profile = Profile::bladeguard_veteran_squad

    # Abilities
    base_model.abilities.append(AbilityKind::oath_of_moment)
    base_model.abilities.append(AbilityKind::bladeguard)
    # Additional temporary “Swords of the Chapter” or “Shields of the Chapter” will be
    # selected in the Fight phase, so we do not permanently append them here.

    # Keywords
    base_model.keywords.append(Keyword::infantry)
    base_model.keywords.append(Keyword::imperium)
    base_model.keywords.append(Keyword::tacticus)
    base_model.keywords.append(Keyword::bladeguard_veteran_squad)
    # Optionally add Adeptus Astartes or Dark Angels if you define them in the Keyword enum

    # Weapons
    base_model.weapons.append(Weapon::heavy_bolt_pistol)
    base_model.weapons.append(Weapon::master_crafted_power_weapon)

    # Add 3 identical models
    for i in range(3):
        squad.models.append(base_model)

    return squad

fun make_vengeful_brethren(UnitVector out, Bool owner) -> Faction:
    # 1) Master Zacharial
    out.append(make_master_zacharial())
    out.back().owned_by_player1 = owner

    # 2) Intercessor Squad (10 models)
    out.append(make_intercessor_squad())
    out.back().owned_by_player1 = owner

    # 3) Hellblaster Squad (5 models)
    out.append(make_hellblaster_squad())
    out.back().owned_by_player1 = owner

    # 4) Bladeguard Veteran Squad (3 models)
    out.append(make_bladeguard_veteran_squad())
    out.back().owned_by_player1 = owner

    return Faction::vengeful_brethren
