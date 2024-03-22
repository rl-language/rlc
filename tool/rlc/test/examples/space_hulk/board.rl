# RUN: python %pyscript/test.py --source %s -i %stdlib --rlc rlc 

import unit 
import bounded_arg
import collections.vector
import machine_learning

using UnitArgType = BInt<0, 10>


ent Board:
  Hidden<BInt<0, 2>[29][28]> map
  HiddenInformation<BInt<0, 5>> command_points
  Vector<Unit> units
  Bool is_done
  BInt<0, 3> current_player
  BInt<0, 11> turn_count

  BInt<0, 10> gsc_killed
  BInt<0, 10> marine_killed

  fun set_is_marine_decision():
    self.current_player = 1

  fun set_is_gsc_decision():
    self.current_player = 2

  fun set_is_random_decision():
    self.current_player = 0

  fun marine_must_act() -> Bool:
    return self.current_player.value == 1

  fun gsc_must_act() -> Bool:
    return self.current_player.value == 2

  fun random_action() -> Bool:
    return self.current_player.value == 0

  fun can_move_to(Unit unit, Direction direction) -> Bool:
    let cost = unit.move_cost(direction)
    if cost is Int:
      if cost > unit.action_points.value:
        return false
      else:
        return self.is_walkable(unit.x.value + direction.to_x(), unit.y.value + direction.to_y())
    else:
      return false

  fun pretty_print_board():
    let to_print : String
    let y = 0 
    while y != 28:
        let x = 0 
        while x != 29:
          if !(self.get_index_of_unit_at(x, y) is Nothing):
            to_print.append('o')
          else if self.map.value[y][x] == 1:
            to_print.append('X')
          else:
            to_print.append(' ')
          x = x + 1
        y = y + 1
        to_print.append('\n')
    print(to_print)


  fun can_move_to(Unit unit, Int absolute_direction) -> Bool:
    let direction : Direction
    direction.value = absolute_direction	
    return self.can_move_to(unit, direction)

  fun is_walkable(Int x, Int y) -> Bool:
    if x < 0:
      return false
    if y < 0:
      return false

    if x >= 28:
      return false

    if y >= 28:
      return false

    let iter = 0
    while iter != self.units.size():
      if self.units.get(iter).x == x and self.units.get(iter).y == y:
        return false 
      iter = iter + 1

    return self.map.value[y][x] == 0

  fun unit_id_is_valid(Int unit_id) -> Bool:
    return unit_id >= 0 and unit_id < 10 and unit_id < self.units.size()

  fun new_turn():
    self.turn_count = self.turn_count + 1
    if self.turn_count == 10:
        self.is_done = true
        return
    let i = 0
    while i < self.units.size():
      self.units.get(i).action_points = self.units.get(i).action_point_allowance()
      i = i + 1

  fun is_in_line_of_sight(Unit source, Unit target) -> Bool:
    if source.x != target.x and source.y != target.y:
      return false
    
    if source.x == target.x:
        let y = min(source.y, target.y)
        while y != max(source.y, target.y):
            if self.map.value[y.value][source.x.value] != 0:
                return false
            y = y + 1
    if source.y == target.y:
        let x = min(source.x, target.x)
        while x != max(source.x, target.x):
            if self.map.value[source.y.value][x.value] != 0:
                return false
            x = x + 1
    # ToDO: implement correctly actual line of sight
    return true

  fun can_shoot(Unit source, Unit target, Bool overwatch) -> Bool:
    if !(source.kind == UnitKind::marine):
      return false

    if target.kind == UnitKind::marine:
      return false

    if !self.is_in_line_of_sight(source, target):
      return false

    if !overwatch and source.get_weapon_ap_cost() > source.action_points.value:
      return false

    return true

  fun shoot_at(Unit source, Unit target, Bool overwatch, BInt<1, 7> roll1, BInt<1, 7> roll2) -> Bool:
    if !overwatch:
      source.action_points = source.action_points - source.get_weapon_ap_cost()
    if overwatch:
      source.is_overwatching = false
    source.is_guarding = false
    return roll1 == 6 or roll2 == 6

  fun can_assault(Unit source) -> Bool:
    if source.action_points < 1:
      return false

    let target_x = source.x + source.direction.to_x()
    let target_y = source.y + source.direction.to_y()

    let maybe_index = self.get_index_of_unit_at(target_x.value, target_y.value)
    if maybe_index is UnitArgType:
      return !(self.units.get(maybe_index.value).kind == source.kind)
    else:
      return false

  fun get_index_of_unit_at(Int x, Int y) -> UnitArgType | Nothing:
    let result : UnitArgType | Nothing
    let content : UnitArgType 
    let i = 0
    while i < self.units.size():
      if self.units.get(i).x == x and self.units.get(i).y == y:
        content = i
        result = content
        return result
      i = i + 1

    result = none()
    return result

  fun score() -> Float:
    if self.marine_killed == 1:
      return -1.0
    let x = self.units.get(0).x
    let y = self.units.get(0).y
    let original_distance = manhattan_distance(21, 2, 4, 13)
    let current_distance = manhattan_distance(x.value, 21, y.value, 4)
    if original_distance > current_distance:
        return (0.9 - (float(current_distance) / 30.0)) + (float(self.gsc_killed.value) / 10.0)
    if original_distance < current_distance:
        return (-0.4 - (float(current_distance) / 30.0)) + (float(self.gsc_killed.value) / 30.0)
    return + (float(self.gsc_killed.value) / 10.0)

fun manhattan_distance(Int x1, Int x2, Int y1, Int y2) -> Int:
  let x = x1 - x2
  let y = y1 - y2 
  if x < 0:
    x = -x
  if y < 0:
    y = -y
  return x + y

fun make_board() -> Board:
  let board : Board
  board.set_is_marine_decision()
  board.is_done = false
  let copy = [
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
  let x = 29
  while x != 0:
    x = x - 1
      let y = 28
      while y != 0:
          y = y - 1
          board.map.value[y][x] = copy[y][x]

  board.command_points.content = 0
  board.command_points.owner = 1
  board.units.append(make_marine(2, 13))
  board.units.get(0).direction = Direction::right
  board.units.append(make_genestealer(27, 13))
  board.units.append(make_genestealer(22, 10))
  board.units.append(make_genestealer(18, 10))
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
    if res is UnitArgType:
        return res.value == 0
    return false

