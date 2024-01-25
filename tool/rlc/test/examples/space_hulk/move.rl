# RUN: rlc %s --type-checked -i %stdlib

import unit 
import board 
import direction

fun absolute_direction_is_valid(Int absolute_direction) -> Bool:
  return absolute_direction >= 0 and absolute_direction < 4

act move_unit(ctx Board board, frm Int unit_id) -> Move:
  board.units.get(unit_id).is_overwatching = false
  board.units.get(unit_id).is_guarding = false 
  frm can_turn = board.units.get(unit_id).kind == UnitKind::genestealer
  frm has_moved = false
  frm is_done = false
  while !is_done:
    actions:
      act turn(Int id, Int absolute_direction) {
        absolute_direction_is_valid(absolute_direction),
        board.units.get(id).can_turn_to(absolute_direction, false),
        unit_id == id,
        can_turn
      }
      board.units.get(unit_id).turn_direction(absolute_direction, true)
      can_turn = false

      act move(Int absolute_direction) {
        absolute_direction_is_valid(absolute_direction),
        board.can_move_to(board.units.get(unit_id), absolute_direction),
        !has_moved
      }
      board.units.get(unit_id).move(absolute_direction)
      has_moved = true

      act end_move()
      is_done = true

  if board.units.get(unit_id).kind.faction() == Faction::genestealer:
    actions:
      act shoot(Int shooter_id, Int target_id) {
        board.unit_id_is_valid(unit_id),
        board.unit_id_is_valid(target_id),
        unit_id == target_id,
        board.can_shoot(board.units.get(unit_id), board.units.get(target_id), true),
        board.units.get(unit_id).is_overwatching
      }
      if board.shoot_at(board.units.get(shooter_id), board.units.get(target_id), true):
        board.units.erase(target_id)

      act do_nothing()

fun test_move_move_forward() -> Bool:
  let board = make_board()
  board.units.get(0).action_points = 6
  let game = move_unit(board, 0)
  game.move(board, 1)
  game.end_move(board)
  return board.units.get(0).x == 3
