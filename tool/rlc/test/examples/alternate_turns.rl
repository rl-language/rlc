# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t
import collections.vector

enum Type:
    water
    fire
    ground
    air

enum Move:
    wildfire # permanent fire / generates fire
    firebreath # damage / fire
    burn_out # dies and obtains permanent fire / fire
    absorb_heat # consumes fire for damage / generates fire 
    
    firepunch # if you have 3 fires, skip the fastest opponent move / generates 1
    spread_the_heat # if there is 5 fire, a permanent fire / generates fire
    stand_your_ground # if it has ground, obtain 1 defense / generates ground
    earth_in_flames # converts a permanent ground in fire / generates ground

    firethrower # adds a opponent fire / generates fire
    fire_blast # requires lot of fire / does nothing
    steam_and_flames # converts a water in a fire / generates fire
    fire_shield # gives defense for each fire / does nothing

    drench # permanent water / generates water 
    waters_of_the_earth # convert water in mud / add water
    shield_up # gets 1 defense / one water
    waves # damages / generates water 

    raindance # remove fire / generates water 
    drowing_the_earth # requires water and earth / add ground
    flow_away # requires 3 waters, forces the opponent out at the end of the turn, remove its permanent colors / one water
    mud_and_mad # requires_ground / generates ground


    earthquake # consume earth / generates nothing
    flow_like_water # if you have exactly 5 waters, treshold one more other moves / add water
    one_with_the_earth # / generates 2 earth
    sea_and_land # generates 1 water / generates 1 earth

enum FighterStats:
    chazard:
        Int attack = 3
        Int defense = 10
        Int max_health = 2
    thayfoosion:
        Int attack = 2
        Int defense = 8
        Int max_health = 1
    chickenbusken:
        Int attack = 3
        Int defense = 1
        Int max_health = 10
    blostise:
        Int attack = 2
        Int defense = 4
        Int max_health = 10
    swambert:
        Int attack = 3
        Int defense = 2
        Int max_health = 8
    emboleon:
        Int attack = 2
        Int defense = 3
        Int max_health = 10



cls Fighter:
    FighterStats stats
    Int current_health
    BoundedVector<Type, 3> types
    BoundedVector<Move, 4> types

cls Team:
    BoundedVector<Fighter, 3> members

fun make_chazard() -> Fighter:
    let to_return : Fighter
    
    to_return.stats = FighterStats::chazard
    to_return.current_health = 10
    to_return.types.append(Type::fire)
    to_return.types.append(Type::air)

    return to_return

fun make_thayfooosion() -> Fighter:
    let to_return : Fighter
    
    to_return.current_health = 8
    to_return.types.append(Type::fire)
    to_return.types.append(Type::fire)
    to_return.types.append(Type::fire)

    return to_return

fun make_chickedbusken() -> Fighter:
    let to_return : Fighter
    
    to_return.current_health = 10
    to_return.types.append(Type::ground)
    to_return.types.append(Type::fire)

    return to_return
    

fun make_blostise() -> Fighter:
    let to_return : Fighter
    
    to_return.current_health = 10
    to_return.types.append(Type::water)
    to_return.types.append(Type::water)

    return to_return

fun make_swambert() -> Fighter:
    let to_return : Fighter
    
    to_return.current_health = 8
    to_return.types.append(Type::water)
    to_return.types.append(Type::ground)

    return to_return

fun make_emboleon() -> Fighter:
    let to_return : Fighter
    
    to_return.current_health = 10
    to_return.types.append(Type::water)
    to_return.types.append(Type::ground)

    return to_return

    
fun main() -> Int:
    return 0
