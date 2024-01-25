# RUN: python %pyscript/test.py --source %s -i %stdlib --rlc rlc

import unit 
import collections.vector

ent Board:
  Int[29][28] map
  HiddenInformation<Int> command_points
  Vector<Unit> units
  Bool is_done

  fun can_move_to(Unit unit, Direction direction) -> Bool:
    let cost = unit.move_cost(direction)
    if cost is Int:
      if cost > unit.action_points:
        return false
      else:
        return self.is_walkable(unit.x + direction.to_x(), unit.y + direction.to_y())
    else:
      return false

  fun can_move_to(Unit unit, Int absolute_direction) -> Bool:
    let direction : Direction
    direction.value = absolute_direction	
    return self.can_move_to(unit, direction)

  fun is_walkable(Int x, Int y) -> Bool:
    if x > 20:
      return false

    if y > 20:
      return false

    let iter = 0
    while iter != self.units.size():
      if self.units.get(iter).x == x and self.units.get(iter).y == y:
        return false 
      iter = iter + 1

    return self.map[y][x] == 0

  fun unit_id_is_valid(Int unit_id) -> Bool:
    return unit_id >= 0 and unit_id < 10 and unit_id < self.units.size()

  fun new_turn():
    let i = 0
    while i < self.units.size():
      self.units.get(i).action_points = self.units.get(i).action_point_allowance()
      i = i + 1

  fun is_in_line_of_sight(Unit source, Unit target) -> Bool:
    # ToDO: implement actual line of sight
    return true

  fun can_shoot(Unit source, Unit target, Bool overwatch) -> Bool:
    if !(source.kind == UnitKind::marine):
      return false

    if target.kind == UnitKind::marine:
      return false

    if !self.is_in_line_of_sight(source, target):
      return false

    if !overwatch and source.get_weapon_ap_cost() > source.action_points:
      return false

    return true

  fun shoot_at(Unit source, Unit target, Bool overwatch) -> Bool:
    if !overwatch:
      source.action_points = source.action_points - source.get_weapon_ap_cost()
    if overwatch:
      source.is_overwatching = false
    source.is_guarding = false
    return roll() == 6 or roll() == 6

  fun can_assault(Unit source) -> Bool:
    if source.action_points < 1:
      return false

    let target_x = source.x + source.direction.to_x()
    let target_y = source.x + source.direction.to_y()

    let maybe_index = self.get_index_of_unit_at(target_x, target_y)
    if maybe_index is Int:
      return !(self.units.get(maybe_index).kind == source.kind)
    else:
      return false

  fun get_index_of_unit_at(Int x, Int y) -> Int | Nothing:
    let result : Int | Nothing
    let i = 0
    while i < self.units.size():
      if self.units.get(i).x == x and self.units.get(i).y == y:
        result = i
        return result
      i = i + 1

    result = none()
    return result

fun make_board() -> Board:
  let board : Board
  board.is_done = false
  board.map = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
  ]
  board.command_points.content = 0
  board.command_points.owner = 1
  board.units.init()
  board.units.append(make_marine(2, 13))
  board.units.get(0).direction = Direction::right
  return board

fun test_is_walkable() -> Bool:
    let board = make_board()
    if board.is_walkable(2, 13):
        return false
    if board.is_walkable(2, 12):
        return false
    if !board.is_walkable(3, 13):
        return false
    return true

fun test_move_right() -> Bool:
    let board = make_board()
    board.units.get(0).action_points = 6
    return board.can_move_to(board.units.get(0), Direction::right)

fun test_connot_move_up() -> Bool:
    let board = make_board()
    board.units.get(0).action_points = 6
    return !board.can_move_to(board.units.get(0), Direction::up)

fun test_get_unit_at() -> Bool:
    let board = make_board()
    let expected_none = board.get_index_of_unit_at(2, 12)
    if !(expected_none is Nothing):
        return false
    let res = board.get_index_of_unit_at(2, 13)
    if res is Int:
        return res == 0
    return false

