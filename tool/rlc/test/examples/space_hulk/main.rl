# RUN: python %pyscript/test.py --source %s -i %stdlib --rlc rlc

import collections.vector
import serialization.to_byte_vector
import serialization.print
import direction
import math.numeric
import unit 
import board 
import move
import action


act roll_melee(frm UnitKind kind) -> RollMelee:
  # ToDo: implement marine sergent
  act roll_dice(frm DiceRoll roll1)
  act roll_dice(frm DiceRoll roll2)
  frm result = max(roll1, roll2)
  if kind == UnitKind::marine:
    return
  act roll_dice(frm DiceRoll roll3)
  result = max(result, roll3)

act do_assault(ctx Board board, frm UnitArgType unit_id) -> Assault:
  ref source = board.units.get(unit_id.value)
  frm maybe_dead_unit : UnitArgType | Nothing
  maybe_dead_unit = none()
  source.is_overwatching = false
  source.is_guarding = false

  source.action_points = source.action_points - 1

  let target_x = source.x + source.direction.to_x()
  let target_y = source.y + source.direction.to_y()

  let maybe_index = board.get_index_of_unit_at(target_x.value, target_y.value)
  if maybe_index is UnitArgType:
    frm index = maybe_index
    frm attacker_direction = board.units.get(index.value).direction

    subaction* source_roll = roll_melee(source.kind) 
    subaction* target_roll = roll_melee(board.units.get(index.value).kind) 
    ref target = board.units.get(index.value)

    frm current_player = board.current_player
    if target.is_guarding:
      board.set_is_marine_decision()
      act guard_reroll(frm Bool do_it)
      act roll_dice(DiceRoll roll)
      if do_it:
        target_roll.result = max(target_roll.result, roll)

    # if defender has not lost, allow to turn
    if source_roll.result <= target_roll.result and !attacker_direction.is_facing(board.units.get(index.value).direction):
      board.current_player = current_player
      act face_attacker(Bool do_it)
      if do_it:
        board.units.get(index.value).direction = attacker_direction.opposite()

    # defender won
    if source_roll.result < target_roll.result:
      maybe_dead_unit = unit_id

    # attacker won
    if source_roll.result > target_roll.result:
      maybe_dead_unit = index

act action_phase(ctx Board board, frm Faction current_faction) -> ActionPhase:
  while true:
    if current_faction == Faction::marine:
      board.set_is_marine_decision()
    else:
      board.set_is_gsc_decision()
    actions:
      act begin_move(UnitArgType unit_id) {
        board.unit_id_is_valid(unit_id.value),
        board.units.get(unit_id.value).action_points > 0,
        board.units.get(unit_id.value).faction() == current_faction
      }
        subaction*(board) movement = move_unit(board, unit_id)

      act turn(UnitArgType unit_id, DirectionArgType absolute_direction){
        board.unit_id_is_valid(unit_id.value),
        board.units.get(unit_id.value).can_turn_to(absolute_direction.value, false),
        board.units.get(unit_id.value).faction() == current_faction
      }
        board.units.get(unit_id.value).turn_direction(absolute_direction.value, false)

      act shoot(UnitArgType unit_id, UnitArgType target_id) {
        board.unit_id_is_valid(unit_id.value),
        board.unit_id_is_valid(target_id.value),
        board.can_shoot(board.units.get(unit_id.value), board.units.get(target_id.value), false),
        board.units.get(unit_id.value).faction() == current_faction,
        !(board.units.get(target_id.value).faction() == current_faction)
      }
        frm unit_id = unit_id
        frm target_id = target_id
        act roll_dice(DiceRoll roll1) 
        frm roll = roll1
        act roll_dice(DiceRoll roll2) 
        if board.shoot_at(board.units.get(unit_id.value), board.units.get(target_id.value), false, roll, roll2):
          board.gsc_killed = board.gsc_killed + 1
          board.units.erase(target_id.value)

      act overwatch(UnitArgType unit_id) {
        board.unit_id_is_valid(unit_id.value),
        board.units.get(unit_id.value).can_overwatch(),
        board.units.get(unit_id.value).faction() == current_faction
      }
        board.units.get(unit_id.value).action_points = board.units.get(unit_id.value).action_points - 2
        board.units.get(unit_id.value).is_overwatching = true
        board.units.get(unit_id.value).is_guarding = false

      act guard(UnitArgType unit_id) {
        board.unit_id_is_valid(unit_id.value),
        board.units.get(unit_id.value).can_guard(),
        board.units.get(unit_id.value).faction() == current_faction
      }
        board.units.get(unit_id.value).action_points = board.units.get(unit_id.value).action_points - 2
        board.units.get(unit_id.value).is_guarding = true
        board.units.get(unit_id.value).is_overwatching = false

      act assault(UnitArgType unit_id) {
        board.unit_id_is_valid(unit_id.value),
        board.can_assault(board.units.get(unit_id.value)),
        board.units.get(unit_id.value).faction() == current_faction
      }
        subaction*(board) assault_frame = do_assault(board, unit_id)
        let to_kill = assault_frame.maybe_dead_unit
        if to_kill is UnitArgType:
          if current_faction == Faction::marine:
              board.gsc_killed = board.gsc_killed + 1
          else:
              board.marine_killed = board.marine_killed + 1
          board.units.erase(to_kill.value)

      act pass_turn()
        return

      act quit()
        return

act play() -> Game:
    frm board = make_board()

    while !(board.is_done):
        board.new_turn()
        subaction*(board) marine_frame = action_phase(board, Faction::marine)
        if board.is_done:
            return
        subaction*(board) genestealer_frame = action_phase(board, Faction::genestealer)

fun gen_printer_parser():
    let state : Game
    let any_action :  AnyGameAction
    gen_python_methods(state, any_action)

fun main() -> Int:
  let state = play()
  let x : UnitArgType
  x = 0
  let gs : UnitArgType
  gs = 3
  let dice : DiceRoll 
  dice = 6
  let m : DirectionArgType 
  m.value = Direction::right.value
  state.quit()
  state.quit()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.quit()
  state.quit()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.quit()
  state.quit()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.quit()
  state.quit()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.quit()
  state.quit()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.quit()
  state.quit()
  m.value = Direction::up.value
  state.turn(x, m)
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.begin_move(x)
  state.move(m)
  state.end_move()
  state.quit()
  gs = 3
  state.assault(gs)
  state.roll_dice(dice)
  state.roll_dice(dice)
  state.roll_dice(dice)
  dice = 1
  state.roll_dice(dice)
  state.roll_dice(dice)
  print_indented(state)
  state.board.pretty_print_board()
  print(state.board.score())
  return int(state.is_done()) - 1

fun test_enumerate() -> Bool:
  let state = play()
  let action : GameBeginMove
  let enumeration = enumerate(action) 

  if enumeration.size() == 10:
    return true
  else:
    return false

fun test_game_marine_can_roll() -> Bool:
  let game = play()
  let gs : UnitArgType
  gs = 3

  let arg : UnitArgType
  arg.value = 0
  game.shoot(arg, gs)

  let roll : DiceRoll
  roll.value = 0

  return can game.roll_dice(roll) 

fun test_game_marine_can_not_roll() -> Bool:
  let game = play()
  let gs : UnitArgType
  gs = 1

  let roll : DiceRoll
  roll.value = 0

  return !can game.roll_dice(roll) 

fun test_game_marine_can_step_forward() -> Bool:
  let game = play()
  let arg : UnitArgType
  arg.value = 0
  game.begin_move(arg)
  let arg2 : DirectionArgType 
  arg2.value = 1
  game.move(arg2)
  game.end_move()
  return game.board.units.get(0).x == 5

fun test_game_marine_can_end_turn() -> Bool:
    let game = play()
    if game.board.units.get(0).action_points != 4:
        return false
    let arg : UnitArgType
    arg.value = 0
    game.begin_move(arg)
    let arg2 : DirectionArgType 
    arg2.value = 1
    game.move(arg2)
    game.end_move()

    if game.board.units.get(0).action_points != 3:
        return false

    game.pass_turn()
    game.pass_turn()
    return game.board.units.get(0).action_points == 4


fun test_max() -> Bool:
  let x = max(3, 4)
  return x == 4

fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4
    let roll : DiceRoll
    roll.value = 6
    if can g.roll_dice(roll):
      return -1
    if g.board.marine_must_act():
      return 0
    if g.board.gsc_must_act():
      return 1
    return 0

fun score(Game g) -> Float:
    return g.board.score()

fun fuzz(Vector<Byte> input):
    let state = play()
    let action : AnyGameAction
    parse_and_execute(state, action, input) 
