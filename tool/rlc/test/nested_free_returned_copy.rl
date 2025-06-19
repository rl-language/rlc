# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import collections.vector

cls Player:
  fun assign(Player other):
    1 == 1

cls State:
  Vector<Player> players

  fun get_current_player() -> Player:
    return self.players.get(0)

fun main() -> Int:
  let state : State
  let player : Player
  state.players.append(player)
  state.get_current_player()
  return 0

