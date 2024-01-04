# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

# The rulebook language offers a coroutine based solution for the problem of writing 
# efficient and readable code that requires lots of user inputs.

# Let us first consider the following `main` function which will use 
# functions defined later in the file. 

# The main function is playing out a game of "odd and evens". 

# As a reminder, the rules are the following: 
# One player predicts either `even` or `odd`, and then both players raise at the same time 
# any number of fingers on one of their hands. 
# If the total sum of raised fingers has the predicted parity, the the first player wins.
# Otherwise the second does.

fun main() -> Int:
  let game_state = odd_and_evens()

  game_state.player_one_predicts_odd(true) 

  game_state.current_player_reveals_hand(6) # fails, you cannot raise 6 fingers on one hand

  game_state.current_player_reveals_hand(4) # success, player 1 has committed 4 fingers
  game_state.current_player_reveals_hand(2) # success, player 2 has committed 2 fingers

  if !game_state.is_done():
    return -1

  if game_state.p1_has_won:
    return -1

  return 0


# The main function begins the simuation of `odd and evens` giving the name 
# `game_state` to the object containing it. Then it invokes three methods that advance 
# the status of the game, allowing the player to try again if the action was illegal. 
# At the end it checks that the game is compleated, 
# and that the expected player has own. It that case it returns 0. -1 otherwise.

# Take a moment to think about how would implement the the `odd_and_evens`, 
# `player_one_predicts_odd`, `player_one_reveal_hand`, `player_two_revel_hand`, `is_done` 
# functions.

# Maybe you thought of a class with some members and some methods, maybe you thought of
# some logic based programming language, maybe you thought of some callback based 
# framework. Here is how rl does it.

# Declares a coroutine, and a type called `Game` which will contain the coroutine 
# variables. Then it starts the coroutine.
act odd_and_evens() -> Game:
  # declares a local variable, which will be saved in the `game_state` variable.
  let p1_has_won = true

  # stop the execution of the coroutine until `player_one_predicts_odd` is invoked. The
  # argument of that function is bound to `first_player_has_selected_odd`
  act player_one_predicts_odd(Bool first_player_has_selected_odd)

  # keep executing the next action until the input is valid
  let p1_choise = -1
  while p1_choise < 0 or p1_choise > 5:
    act current_player_reveals_hand(Int p1_finger_count)
    p1_choise = p1_finger_count

  let p2_choise = -1
  while p2_choise < 0 or p2_choise > 5:
    act current_player_reveals_hand(Int p2_finger_count) 
    p2_choise = p2_finger_count

  # check who has won. Sum is not saved in the coroutine frame, it is not used in between 
  # actions, it does not need to take space! 
  let sum = p1_choise + p2_choise
  if sum % 2 != int(first_player_has_selected_odd):
    p1_has_won = false 

# What is going here? It this `coroutine action` we are expressing control flow, as well 
# as declaring global functions that can be executed. 
# Every time the control flow reaches a action statement (lines 56, 61 and 66),  the 
# control flow stops and will resume only when the action we have just reached is invoked.

# Furthermore, as shown by the vscode autocomplete mechanism, actions are statically 
# checked. You cannot invoke a actions with the wrong arguments, or a action that does 
# not exists. Try it!

# The code is compiled with asserts, try swapping line 18 and 20. The program will crash. 
# That is intentional and shows that the simulation knows what is the correct internal 
# state.

# Finally, no dynamic allocation or syscall happens in this piece of code. 
# This piece of code is as performant as it would have if you hand written it in C.

# Here is the same coroutine, except we assume that the players will not provide impossible
# arguments, so instead of looping until we recieve a valid one, we simply assert their 
# values. Try invoking this new coroutine at line 16. As long as line 20 exists, the 
# program will crash. 
#
# Think back at your predicted implementation in another language. 
# Would have been this easy to write?
act odd_and_evens_asserts() -> GameWithAsserts:
  let p1_has_won = true

  act player_one_predicts_odd(Bool first_player_has_selected_odd)
  act current_player_reveals_hand(Int p1_choise) { p1_choise <= 5 and p1_choise >= 0 }
  act current_player_reveals_hand(Int p2_choise) { p2_choise <= 5 and p2_choise >= 0 }

  let sum = p1_choise + p2_choise
  if sum % 2 != int(first_player_has_selected_odd):
    p1_has_won = false 
