# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act playChess() -> Chess:
  act move1Chess()
  act move2Chess()


act playDama() -> Dama:
  act move1Dama()
  act move2Dama()


act chooseGame() ->GameChooser:
  act choose(Bool isChess)
  frm game : Chess | Dama
  if isChess:
    game = playChess()
  else:
    game = playDama()

  subaction* game

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if alternative subaction statements work

const game = $.chooseGame();
game.choose(true);
game.move1Chess();
game.move2Chess();
assert.strictEqual(game.is_done(), true);

game._free();
$.Std._detectMemoryLeaksDoNotUse();