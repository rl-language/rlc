# RUN: python %pyscript/test.py %s -i %stdlib --rlc rlc 

import unit 
import bounded_arg
import collections.vector
import machine_learning

using UnitArgType = BInt<0, 27>


cls Board:
  BInt<0, 5>[29][28] map
  HiddenInformation<BInt<0, 5>> command_points
  BoundedVector<Unit, 27> units
  Bool is_done
  BInt<0, 3> current_player
  BInt<0, 16> turn_count

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

  fun is_facing_door(Unit u) -> Bool:
    let x = u.x_of_cell_in_front()
    let y = u.y_of_cell_in_front()
    if x < 0:
      return false
    if y < 0:
      return false

    if x >= 28:
      return false

    if y >= 28:
      return false

    return self.map[y][x] == 4 or self.map[y][x] == 3

  fun toggle_door(Unit u):
    let x = u.x_of_cell_in_front()
    let y = u.y_of_cell_in_front()
    if self.map[y][x] == 4:
       self.map[y][x] = 3
    else if self.map[y][x] == 3:
       self.map[y][x] = 4

  fun pretty_print_board():
    let to_print : String
    let y = 0 
    while y != 28:
        let x = 0 
        while x != 29:
          let index = self.get_index_of_unit_at(x, y)
          if index is UnitArgType:
            if self.units.get(index.value).is_marine():
              to_print.append('M')
            else:
              to_print.append('G') 
          else if self.map[y][x] == 1:
            to_print.append('X')
          else if self.map[y][x] == 2:
            to_print.append('S')
          else if self.map[y][x] == 4:
            to_print.append('#')
          else if self.map[y][x] == 3:
            to_print.append(' ')
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

    return self.map[y][x] == 0 or self.map[y][x] == 3

  fun unit_id_is_valid(Int unit_id) -> Bool:
    return unit_id >= 0 and unit_id < self.units.size()

  fun new_turn():
    self.turn_count = self.turn_count + 1
    if self.turn_count == 15:
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
        let y = min(source.y, target.y).value + 1
        while y <  max(source.y, target.y).value:
            if !self.is_walkable(source.x.value, y): 
                return false
            y = y + 1

    if source.y == target.y:
        let x = min(source.x, target.x).value + 1
        while x < max(source.x, target.x).value:
            if !self.is_walkable(x, source.y.value): 
                return false
            x = x + 1
    # ToDO: implement correctly actual line of sight
    return true

  fun can_shoot(Unit source, Unit target, Bool overwatch, Bool is_bolter_free_shoot) -> Bool:
    if !(source.kind == UnitKind::marine):
      return false

    if target.kind == UnitKind::marine:
      return false

    if !self.is_in_line_of_sight(source, target):
      return false

    if is_bolter_free_shoot:
      return true

    if !overwatch and source.get_weapon_ap_cost() > source.action_points.value:
      return false

    if overwatch and !source.is_overwatching:
      return false

    if source.is_jammed:
      return false

    return true

  fun shoot_at(Unit source, Unit target, Bool overwatch, Bool is_bolter_free_shoot, BInt<1, 7> roll1, BInt<1, 7> roll2) -> Bool:
    if is_bolter_free_shoot:
      source.is_overwatching = false
      source.is_guarding = false
      return roll1 == 6 or roll2 == 6

    if !overwatch:
      source.action_points = source.action_points - source.get_weapon_ap_cost()
      source.is_overwatching = false
    if overwatch:
      if roll1 == roll2:
        source.is_jammed = true
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

  fun _single_marine_score(Unit u) -> Float:
    let x = u.x
    let y = u.y
    let current_distance = manhattan_distance(x.value, 21, y.value, 7)
    return float(current_distance) / 20.0

  fun _single_gsc_score(Unit marine, Unit gsc) -> Float:
    let d = manhattan_distance(marine.x.value, gsc.x.value, marine.y.value, gsc.y.value)
    return 1.0 - (float(d) / 60.0) 

  fun _all_marine_average_score() -> Float:
    let sum : Float
    let count : Float
    let x = 0
    while x < 5:
        if self.units.get(x).is_marine():
            sum = sum + self._single_marine_score(self.units.get(x))
            count = count + 1.0
        x = x + 1
    if count == 0.0:
      return 0.0
    return 1.0 - (sum / count)

  fun _all_gsc_average_score() -> Float:
    let sum : Float
    let count : Float
    let x = 0
    while x < self.units.size():
        if !self.units.get(x).is_marine():
            sum = sum + (float(manhattan_distance(self.units.get(x).x.value, 21, self.units.get(x).y.value, 7)) / 30.0)
            count = count + 1.0
        x = x + 1
    if count == 0.0:
        return 0.0
    return 1.0 - (sum / count)

  fun any_marine_won() -> Bool:
    let x = 0
    while x < 5:
      if self.units.get(x).is_marine():
        ref marine = self.units.get(x)
        if manhattan_distance(marine.x.value, 21, marine.y.value, 7) == 0:
          return true
      x = x + 1
    return false

  fun euristics() -> Float:
    let score = self._all_marine_average_score()
    return score

  fun gsc_euristics() -> Float:
    let score = float(self.marine_killed.value) + self._all_gsc_average_score()
    return score

  fun score() -> Float:
    if self.any_marine_won():
      return 10.0
    if self.marine_killed == 5:
      return -10.0
    return self.euristics()

  fun gsc_score() -> Float:
    if self.marine_killed == 5:
      return 10.0
    if self.any_marine_won():
      return -10.0
    return self.gsc_euristics()

  fun get_spawn_point(Int spawn_index, Int x_out, Int y_out):
    let x = 29
    while x != 0:
      x = x - 1
        let y = 28
        while y != 0:
          y = y - 1
          if self.map[y][x] == 2:
            if spawn_index == 0:
              x_out = x
              y_out = y
              return
            spawn_index = spawn_index - 1

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
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 2, 1, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
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
          board.map[y][x] = copy[y][x]

  board.command_points.value = 0
  board.command_points.owner = 1
  board.units.append(make_marine(3, 13))
  board.units.append(make_marine(5, 13))
  board.units.append(make_marine(4, 13))
  board.units.append(make_marine(2, 13))
  board.units.append(make_marine(1, 13))
  board.units.get(0).direction = Direction::right
  board.units.get(1).direction = Direction::right
  board.units.get(2).direction = Direction::right
  board.units.get(3).direction = Direction::right
  board.units.get(4).direction = Direction::right
  return board


fun test_is_walkable() -> Bool:
    let board = make_board()
    if !board.is_walkable(7, 13):
        return false
    if board.is_walkable(2, 12):
        return false
    # door
    if board.is_walkable(6, 13):
        return false
    return true

fun test_move_right() -> Bool:
    let board = make_board()
    board.map[13][6] = 0
    board.units.get(1).action_points = 6
    return board.can_move_to(board.units.get(1), Direction::right)

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
        return res.value == 3
    return false

