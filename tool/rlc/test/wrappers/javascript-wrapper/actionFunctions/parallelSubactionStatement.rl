# RUN: %run_wasm32_test
# RUN: %run_wasm64_test

#--- main.rl
act playChess() -> Chess:
  act move1Chess()
  act move2Chess()


act playDama() -> Dama:
  act move1Dama()
  act move2Dama()


act playInParallel() ->PlayParellel:
  frm chess = playChess()
  frm dama = playDama()
  subaction* chess, dama

#--- test.mjs
import assert from 'assert';
import * as $ from './wrapper.mjs';

//Checking if parallel subaction statements work

const game = $.playInParallel();
game.move1Chess();
game.move1Dama();
game.move2Dama();
game.move2Chess();
assert.strictEqual(game.is_done(), true);

game._free();
$.Std._detectMemoryLeaksDoNotUse();