# RUN: rlc %s --type-checked -i %stdlib

import collections.hidden_information
import direction 
import math.numeric
import none
import bounded_arg

fun roll() -> BInt<1, 7>:
  # ToDo: implement random rolls
  let x : BInt<1, 7>
  x = 6
  return x

enum Faction:
  genestealer
  marine

  fun equal(Faction r) -> Bool:
    return self.value == r.value

fun write_in_observation_tensor(Faction obj, Vector<Float> output, Int index) :
    write_in_observation_tensor(obj.value, 0, 2, output, index)

fun size_as_observation_tensor(Faction obj) -> Int :
    return 2

enum UnitKind:
  genestealer
  marine
  blip

  fun equal(UnitKind r) -> Bool:
    return self.value == r.value

  fun faction() -> Faction:
    if self == UnitKind::marine:
      return Faction::marine
    return Faction::genestealer

  fun action_point_allowance() -> Int:
    if self.value == UnitKind::marine.value:
      return 4
    return 6

fun write_in_observation_tensor(UnitKind obj, Vector<Float> output, Int index) :
    write_in_observation_tensor(obj.value, 0, 3, output, index)

fun size_as_observation_tensor(UnitKind obj) -> Int :
    return 3

ent Unit:
  Bool is_overwatching
  Bool is_guarding 
  UnitKind kind
  HiddenInformation<BInt<0, 4>> genestealer_count
  Direction direction
  BInt<0, 7> action_points
  Bool has_acted
  BInt<0, 30> x
  BInt<0, 30> y

  fun action_point_allowance() -> Int:
    return self.kind.action_point_allowance()

  fun faction() -> Faction:
    return self.kind.faction()

  fun move_cost(Direction dir) -> Int | Nothing:
    let result : Int | Nothing
    if self.kind == UnitKind::blip:
      result = 1
      return result

    if self.kind == UnitKind::marine:
      if self.direction.is_move_forward(dir):
        result = 1
        return result
      if self.direction.is_move_backward(dir):
        result = 2
        return result

    if self.kind == UnitKind::genestealer:
      if self.direction.is_move_backward(dir):
        result = 2
        return result
      result = 1
      return result
    result = none()
    return result

  fun turn_cost(Direction dir, Bool genestealer_free_turn) -> Int | Nothing:
    let result : Int | Nothing
    if self.direction.is_move_forward(dir):
      result = none() 
      return result

    if self.direction.is_move_backward(dir) and genestealer_free_turn:
      result = none()
      return result

    if self.direction.is_move_backward(dir) and self.kind == UnitKind::marine:
      result = none()
      return result

    result = 1
    return result

  fun can_turn_to(Int absolute_direction, Bool genestealer_free_turn) -> Bool:
    let direction : Direction
    direction.value = absolute_direction
    let cost = self.turn_cost(direction, genestealer_free_turn)
    if cost is Int:
      return self.action_points > cost
    return false

  fun get_weapon_ap_cost() -> Int:
    return 1

  fun turn_direction(Int absolute_direction, Bool genestealer_free_turn):
    let direction : Direction
    direction.value = absolute_direction	

    self.is_overwatching = false
    self.is_guarding = false
    let cost = self.turn_cost(direction, genestealer_free_turn)
    if cost is Int:
      self.action_points = self.action_points - cost
      self.direction = direction 
      return 

  fun move(Int absolute_direction):
    let direction : Direction
    direction.value = absolute_direction	

    self.x = self.x + direction.to_x()
    self.y = self.y + direction.to_y()
    let cost = self.move_cost(direction)
    if cost is Int:
      self.action_points = self.action_points - cost 

  fun can_overwatch() -> Bool:
    return self.action_points >= 2 and self.kind == UnitKind::marine

  fun can_guard() -> Bool:
    return self.action_points >= 2 and self.kind == UnitKind::marine and !self.is_guarding

  fun roll_melee() -> BInt<1, 7>:
    # ToDo: implement marine sergent
    if self.kind == UnitKind::marine:
      return max(roll(), roll())
    return max(roll(), max(roll(), roll()))

fun make_blip(Int blip_count, Int x, Int y) -> Unit:
  let unit : Unit
  unit.kind = UnitKind::blip
  unit.genestealer_count.content = blip_count
  unit.genestealer_count.owner = 2
  unit.x = x
  unit.y = y
  return unit

fun make_marine(Int x, Int y) -> Unit:
  let unit : Unit
  unit.kind = UnitKind::marine
  unit.x = x
  unit.y = y
  unit.is_overwatching = false
  return unit

fun make_genestealer(Int x, Int y) -> Unit:
  let unit : Unit
  unit.kind = UnitKind::genestealer
  unit.x = x
  unit.y = y
  return unit

fun test_unit_kind_faction() -> Bool:
    if !(UnitKind::marine.faction() == Faction::marine):
        return false
    if !(UnitKind::genestealer.faction() == Faction::genestealer):
        return false
    if !(UnitKind::blip.faction() == Faction::genestealer):
        return false
    return true

fun test_turn_cost() -> Bool:
    let marine = make_marine(0, 0)
    marine.direction = Direction::left
    return marine.turn_cost(Direction::right, false) is Nothing 

fun test_turn_cost2() -> Bool:
    let marine = make_marine(0, 0)
    marine.direction = Direction::left
    let cost = marine.turn_cost(Direction::up, false)
    if cost is Int:
        return cost == 1
    return false

fun test_marine_can_turn_left() -> Bool:
  let marine = make_marine(0, 0)
  marine.direction = Direction::up
  return marine.turn_cost(Direction::left, false) is Int

fun test_marine_cannot_turn_back() -> Bool:
  let marine = make_marine(0, 0)
  marine.direction = Direction::up
  return marine.turn_cost(Direction::down, false) is Nothing

fun test_move_cost() -> Bool:
    let marine = make_marine(1, 2)
    marine.direction = Direction::right
    let res = marine.move_cost(Direction::right)
    if res is Int:
        return res == 1
    return false
