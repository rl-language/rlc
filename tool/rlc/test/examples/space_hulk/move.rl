# RUN: rlc %s --type-checked -i %stdlib

import unit 
import board 
import direction
import bounded_arg

using UnitArgType = BInt<0, 10>
using DirectionArgType = BInt<0, 4>

act move_unit(ctx Board board, frm UnitArgType unit_id) -> Move:
  board.units.get(unit_id.value).is_overwatching = false
  board.units.get(unit_id.value).is_guarding = false 
  frm can_turn = board.units.get(unit_id.value).kind == UnitKind::genestealer
  frm has_moved = false
  frm is_done = false
  while !is_done:
    actions:
      act turn(UnitArgType id, DirectionArgType absolute_direction) {
        board.units.size() > id.value,
        0 <= id.value,
        board.units.get(id.value).can_turn_to(absolute_direction.value, can_turn),
        unit_id == id,
        can_turn
      }
      board.units.get(unit_id.value).turn_direction(absolute_direction.value, can_turn)
      can_turn = false

      act move(DirectionArgType absolute_direction) {
        board.can_move_to(board.units.get(unit_id.value), absolute_direction.value),
        !has_moved
      }
      board.units.get(unit_id.value).move(absolute_direction.value)
      has_moved = true

      act end_move() 
      if !has_moved:
        board.units.get(unit_id.value).action_points = board.units.get(unit_id.value).action_points - 1
      is_done = true

  if board.units.get(unit_id.value).kind.faction() == Faction::genestealer:
    board.is_marine_decision = false 
    actions:
      act shoot(UnitArgType unit_id, UnitArgType target_id) {
        board.unit_id_is_valid(unit_id.value),
        board.unit_id_is_valid(target_id.value),
        unit_id == target_id,
        board.can_shoot(board.units.get(unit_id.value), board.units.get(target_id.value), true),
        board.units.get(unit_id.value).is_overwatching
      }
      if board.shoot_at(board.units.get(unit_id.value), board.units.get(target_id.value), true):
        board.units.erase(target_id.value)
        board.gsc_killed = board.gsc_killed + 1

      act do_nothing()

fun test_move_move_forward() -> Bool:
  let board = make_board()
  board.units.get(0).action_points = 6
  let arg : UnitArgType
  arg.value = 0
  let game = move_unit(board, arg)
  let arg2 : DirectionArgType 
  arg2.value = 1
  game.move(board, arg2)
  game.end_move(board)
  return board.units.get(0).x == 3
