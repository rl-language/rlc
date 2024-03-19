# RUN: python %pyscript/test.py --source %s -i %stdlib --rlc rlc

import action

enum Direction:
  left
  right
  up
  down

  fun opposite() -> Direction:
    if self == Direction::left: 
      return Direction::right 
    if self == Direction::right:
      return Direction::left
    if self == Direction::up: 
      return Direction::down
    if self == Direction::down:
      return Direction::up
    return Direction::left

  fun is_facing(Direction r) -> Bool:
    return self == r.opposite()

  fun equal(Direction r) -> Bool:
    return self.value == r.value

  fun to_x() -> Int:
    if self == Direction::left:
      return -1
    if self == Direction::right:
      return 1
    return 0

  fun to_y() -> Int:
    if self == Direction::up:
      return -1
    if self == Direction::down:
      return 1
    return 0

  fun is_move_backward(Direction v2) -> Bool:
    return self.opposite() == v2

  fun is_move_forward(Direction v2) -> Bool:
    return self == v2

  fun is_move_sideways(Direction v2) -> Bool:
    return !self.is_move_backward(v2) and !self.is_move_forward(v2)


fun write_in_observation_tensor(Direction obj, Vector<Float> output, Int index) :
    write_in_observation_tensor(obj.value, 0, 4, output, index)

fun size_as_observation_tensor(Direction obj) -> Int :
    return 4

fun test_direction_to_x() -> Bool:
    if Direction::right.to_x() != 1:
        return false
    if Direction::left.to_x() != -1:
        return false
    if Direction::up.to_x() != 0:
        return false
    if Direction::down.to_x() != 0:
        return false
    return true

fun test_direction_opposite() -> Bool:
    if !(Direction::right.opposite() == Direction::left):
        return false
    if !(Direction::up.opposite() == Direction::down):
        return false
    return true

fun test_is_move_forward() -> Bool:
    return Direction::right.is_move_forward(Direction::right)
