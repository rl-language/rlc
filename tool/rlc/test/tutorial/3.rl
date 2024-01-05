# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

# In the previous example we saw the relationship between how a `action function` is 
# declared and how it can be used. Now we will investigate how you can compose and 
# reuse actions to create complex and mantainable programs.

# We already saw the basic `action function` structure, where one can declare action with suspension points

act odd_and_evens_asserts() -> Game:
  let p1_has_won = true

  act player_one_predicts_odd(Bool first_player_has_selected_odd)
  act current_player_reveals_hand(Int p1_choise) { p1_choise <= 5 and p1_choise >= 0 }
  act current_player_reveals_hand(Int p2_choise) { p2_choise <= 5 and p2_choise >= 0 }

  let sum = p1_choise + p2_choise
  if sum % 2 != int(first_player_has_selected_odd):
    p1_has_won = false 


# the first thing one may wish to do is to wrap a action into another one in such a way 
# some inner action is invoked before the user can do anything else. 

fun p1_always_pick_odd() -> Game:
  let frame = odd_and_evens_asserts()
  frame.player_one_predicts_odd(true)
  return frame

fun p1_always_pick_odd_test() -> Bool:
  let frame = p1_always_pick_odd()

  # if you uncomment the next line, the program will crashes!
  # frame.player_one_predicts_odd(true)
  frame.current_player_reveals_hand(2)
  frame.current_player_reveals_hand(3)
  return frame.p1_has_won

# right cannot know that the action player_one_predicts_odd does not make any sense to be 
# invoked on the result of p1_always_pick_odd, so the user may decide to wrap it inside 
# another action 

act p1_always_pick_odd_safe() -> GameWithOneLessAction:
  let inner_frame = p1_always_pick_odd()
  while !inner_frame.is_done():
    act current_player_reveals_hand(Int choise) { choise <= 5 and choise >= 0 }
    inner_frame.current_player_reveals_hand(choise)

fun p1_always_pick_odd_safe_test() -> Bool:
  let frame = p1_always_pick_odd_safe()

  # if you uncomment the next line, the program will not compile! 

  # frame.player_one_predicts_odd(true)

  frame.current_player_reveals_hand(2)
  frame.current_player_reveals_hand(2)
  return frame.inner_frame.p1_has_won

# well, that is not very nice toh, we achieved encapsulation, but we are forced to copy 
# paste every time the actions to be able to dispatch them to the inner frame. 
# Thankfully, we have solution, the subaction statement, which does this job for you
# subaction will allow the user of this action to execute a single action before resuming 
# the execution from the line after the subaction statement. subaction* resume the 
# execution only when the underlying action is compleated.

act maybe_play_a_game() -> MaybeGame:
  act do_you_want_to_play(Bool i_do) 
  if !i_do:
    return

  subaction* inner_frame = p1_always_pick_odd()

fun maybe_play_a_game_test() -> Bool:
  let frame = maybe_play_a_game()
  frame.do_you_want_to_play(true)
  frame.current_player_reveals_hand(2)
  frame.current_player_reveals_hand(3)
  return frame.inner_frame.p1_has_won

# right now the subaction statement is not very flexible and does not allow to the user 
# to do things one may expect, such as forbidding to invoke a particular action, 
# or add extra requirements. These limitations are a not intrinsic limits of the system, 
# we are simply waiting for user feedback before committing to one particular 
# syntax or the other.

# Another important feature is the ability of executing alternative actions, that is achieved 
# with the actions statement

act guess_a_number() -> GuessGame:
  let guessed : Int
  actions:
    act guess_4() 
    guessed = 4

    act guess_10() 
    guessed = 10

# you can invoke any of the two functions and the game will resume from there

fun guess_a_number_test() -> Int:
  let frame = guess_a_number()

  # only one of the two can be not commented
  frame.guess_4()
  # frame.guess_10()
  return frame.guessed

fun main() -> Int:
  if !p1_always_pick_odd_test():
    return -1

  if p1_always_pick_odd_safe_test():
    return -2

  if !maybe_play_a_game_test():
    return -3

  if guess_a_number_test() != 4:
    return -4

  return 0
