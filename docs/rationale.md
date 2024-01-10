# The RL language rationale.

The `Rulebook` (or `rl` for short) is a domain specific-language that tries to reduce the complexity of writing simulations, such as those used in reinforcement learning, game programming, and similar other domains. This document explains the rationale behind it.

If you want to jump directly into the code, try it out as described in the main page instead instead. If you want to see a quick description of what the `rlc` tool is capable of, instead of the `rl` language, you can see video [here](https://www.youtube.com/watch?v=tMnBo3TGIbU). A less pratical and more rambly philosofical description of why `rl` is usefull can be found [here](./philosophy.md)

### The Complexity of writing games and rule heavy simulations.

Most games and many simulations have a very programmatic nature, yet they rarely are implemented in a way that resembles their procedural essence. Consider the game Tic Tac Toe, Wikipedia specifies the rules of the game as the following:
> Tic-tac-toe [...] is a paper-and-pencil game for two players who take turns marking the spaces in a three-by-three grid with X or O. The player who succeeds in placing three of their marks in a horizontal, vertical, or diagonal row is the winner.

How would you implement it in your favorite language?

Maybe you thought of a python function such as:
```python
def tic_tac_toe() -> Winner:
  board = TicTacToeBoard()
  while not board.is_full():
    x = input()
    y = input()
    assert not board.is_set(x, y)

    board.set_as_current_player(x, y)
    if board.three_in_a_line():
      return Winner(board.current_player)
    board.change_current_player()
  return None
```

This implementation closely mirrors the `Wikipedia` description and is very straightforward for a human being to reason about it.

As written right now, it is almost useless.

Maybe you are a reinforcement learning engineer and wish to use that function to let an AI system learn how to play it. Maybe you are a game programmer and want to integrate it into your game.
Let us ignore the low-hanging fruit of performances that would push you to rewrite the code in another language and focus instead on the issue that would be common to all languages:

When the function suspends at the line `x = input()` and `y = input()`, the state of game:
* **is not inspectable**: you cannot trivially inspect the `board` variable from outside this function.
* **is not serializable**: you cannot copy the state of the game and resume it somewhere else unless you fork the entire process.
* **is not precondition checkable**: you cannot check if `x` or `y` are valid without providing them on standard input and letting the game crash.
* **controls the main loop**: the game takes over the main loop of the program until the game terminates. If your application wishes to retain control over it, the `tic_tac_toe` function must be run in a different thread or process.

These issues are clearly unacceptable, and any experienced programmer will instinctively know that the usable implementation will be more similar to something like:


```python
class TicTacToe:
  def __init__(self):
    self.board = TicTacToeBoard()
    self.current_player = 0
    self.next_resumption_point = NormalTurn
    self.winner = None

  def is_done(self) -> Bool:
    return self.next_resumption_point == Ended

  def can_mark(self, x: int, y, int) -> Bool:
    return self.next_resumpion_point == NormalTurn and
           x >= 0 and x < 3 and y >= 3 and y <= 3 and
           not self.board.is_set(x, y)

  def mark(self, x: int, y: int):
    assert self.can_mark(x, y)
    self.board.set_marked_by_current_player(x, y)

    if self.board.three_in_a_line():
      self.next_resumption_point == Ended
      self.winner = self.current_player
    else:
      self.current_player = (self.current_player + 1) % 2
```

Now we have a usable implementation!
* you can inspect a `TicTacToe`object and figure out who is the current player and so on.
* you can copy, save, and restore a game before it is completed.
* you can check if an action can be executed with some given parameters using the method `can_mark`
* you retain control over the main loop and can use this class as follows

```python
state = TicTacToe()

if state.can_mark(1, 2):
  state.mark(1, 2)

if state.is_done():
  print "the game is over"
```
Indeed, this is exactly how game code and code similar to games is written. Here is the [TicTacToe](https://github.com/google-deepmind/open_spiel/blob/master/open_spiel/games/tic_tac_toe/tic_tac_toe.cc#L95) apply action from `Google Deepmind open_spiel`
```cpp
void TicTacToeState::DoApplyAction(Action move) {
  SPIEL_CHECK_EQ(board_[move], CellState::kEmpty);
  board_[move] = PlayerToState(CurrentPlayer());
  if (HasLine(current_player_)) {
    outcome_ = current_player_;
  }
  current_player_ = 1 - current_player_;
  num_moves_ += 1;
}

std::vector<Action> TicTacToeState::LegalActions() const {
  if (IsTerminal()) return {};
  // Can move in any empty cell.
  std::vector<Action> moves;
  for (int cell = 0; cell < kNumCells; ++cell) {
    if (board_[cell] == CellState::kEmpty) {
      moves.push_back(cell);
    }
  }
  return moves;
}
```
That is pretty much identical to our Python implementation, except it has a function that returns all legal actions instead of a function that tells you if an action is legal or not.

Indeed, this is the case for most projects trying to solve this issue:
* The [Game Description Language](https://en.wikipedia.org/wiki/Game_Description_Language) also distinguishes `legal moves`, `update rules`, and `termination`.
* Games with millions of copies sold, such as [CK2](https://ck2.paradoxwikis.com/Ze_Lunatic_(Conclave)) have their own custom language where they can express `events` that are composed of `trigger conditions` that must be satisfied before they can manifest, `decisions` the player can select and `effects` that are applied if that `decision` is taken, and `flags` used to coordinate different events.

We live in a world where billion dollar companies write code for custom hardware accellerators in Python and then turn around and implement TicTacToe in C.

The four properties identified before, `inspectability`, `serializability`, `precondition checkability`, and `no main loop ownership` generate this pattern discovered again and again by different programmers in different contexts. Simulations and games that exist in the mind of developers as normal-looking programs end up rewritten in such a way that those four properties are respected, by explicating the underlying control flow diagram that describes them.

We assert that the current state of the art is not sustainable because unrolling the state machines involved in large programs generate a combinatorial explosion in program complexity that makes them hard to refactor and reason about.

## How RL does it

Here is how [TicTacToe](https://github.com/drblallo/rlc/blob/master/tool/rlc/test/tic_tac_toe.rl) is implemented in the `rlc` (`rl compiler`) test suite.

The code that implements the game is:
```python
act play() -> TicTacToe:
	let board : Board
	while !board.full():
		act mark(Int x, Int y) {
			x < 3,
			x >= 0,
			y < 3,
			y >= 0,
			board.get(x, y) == 0
		}
		board.set(x, y, board.current_player())
		if board.three_in_a_line_player(board.current_player()):
			return

		board.next_turn()
```

And the program main function is:

```python
fun main() -> Int:
	# creates a new game
	let game = play()
	game.mark(0, 0)
	# X _ _
	# _ _ _
	# _ _ _
	game.mark(1, 0)
	# X O _
	# _ _ _
	# _ _ _
	game.mark(1, 1)
	# X O _
	# _ X _
	# _ _ _
	game.mark(2, 0)
	# X O O
	# _ X _
	# _ _ _
	game.mark(2, 2)
	# X O O
	# _ X _
	# _ _ X

	# returns 1 because player 1 indeed
	# had three marks in a line
	return int(game.board.three_in_a_line())
```

What is going on here? The rules of the game are written just like the naive Python function at the start of the document, but the main function is written as we had written the explicit class.
This is exactly what `rlc` does: **it takes a procedural description of a simulation, and returns a library that behaves as if it was a class**.

The line`act play() -> TicTacToe:`declares an `action procedure`, which the compiler will translate into something that behaves like a class declaration for the class `TicTacToe`, and emits as well a function declaration `fun play() -> TicTackToe`.

The line `act mark(Int x, Int y) {` is not an `action procedure`, it is instead an `action statement` because it appears within a function. What it does is declare 2 functions: one is `fun mark(Int x, Int y)  -> Void` and the second one is `fun can_mark(Int x, Int y) -> Book`.

Then the compiler inspects the rest of the `action procedure` and by powerful magic, it moves the content inside the just declared functions. At the end of the compilation, the library will look something like this:

```python
# What follow is pseudocode, it all happens inside the
# compiler and the output is assembly, not python or other rl.
# It will be as efficient as having written this in C instead of python

class TicTacToe:
  def __init__(self):
    self.board = Board()

    if !self.board.is_full(): # copied from the loop in the original program
      self.resumption_index = 0 # 0 means we reached the first action statement, mark
    else:
      self.resumption_index = -1 # -1 means we reached the end of the function

  def can_mark(self, x: int, y: int) -> Bool:
    # implied by `act mark` being rechable from the entry point of `play()`
    if self.resumption_index != 0:
      return False

    # taken from the mark action statement
    return x < 3 and x >= 0 and y < 3 and y >= 0 and board.get(x, y) == 0

  def mark(self, x: int, y: int):
    assert self.can_mark(x, y) # can be disabled for extra performances

    # taken from the lines that follow `act mark` in the rl code
    self.board.set(x, y, board.current_player())
	if board.three_in_a_line_player(board.current_player()):
		# implied by the fact that there was a return in the original code
	    self.resumption_index = -1
		return

	board.next_turn()
    # emit the condition of the loop again
    if !self.board.is_full():
      self.resumption_index = 0
    else:
      self.resumption_index = -1

 def is_done(self) -> bool:
   return self.resumption_index == 1
```

This is it, the whole Raison d'etre of the `rl` language is just that, to automatize the tedious and error-prone process of translating a procedure-like description of a simulation into a class-like description. It may look like not a lot, but it took more than 30.000 lines of code to have it working and easy to use.

## Building on top of it
We saw that `rl` has been created to solve the 4 issues of `inspectability`, `serializability`, `precondition checkability`, and `no main loop ownership` described earlier.  Of course, having a tool able to perform this transformation is not the end of the story. Since that simple implementation `TicTacToe` describes in a compact way
* Initial condition
* Terminal conditions
* Actions, and actions parameters and actions preconditions

then instrumentation can be written to generically exploit one or more of these elements. For example, since the compiler knows everything about how that simulation may evolve, even without human intervention, we can:
* generate the wrapper for the simulation in different languages and for different frameworks (C and Python currently supported)
* generate fuzzers that try random valid moves and look for bugs (currently being developed)
* generate the network layer that allows to remotely interact with the simulation (future work)
* statically prove that some actions can never be executed and thus the code is not right (already able to do so in simple situations)
* automatically serialize and deserialize the state of the simulation (already implemented)

###  Conclusion

In this document we described the issue `rl` is trying to tackle, how `rl` does it, and what extra benefit one obtains when using `rl`. If you want to try it out in a live environment, go [here](not_supported_yet).



### FAQ:

##### But is it fast?
`Rlc` is a `LLVM` based compiler, the performances will be comparable as having written the program in `C`.

##### But is it easy to learn?
`Rl` strives to be as easy to use as python. We provide a plugin to have language support in vscode too.

##### But is it garbage collected?
No, we emit destructors similarly to `cpp` and `rust`. We are considering if and how to adopt a full borrow checker such as the `rust`.

##### But is it safe?
`Rl` is fully type-checked, consider it as safe as `C#` or `Java`, indeed it can only work because it is type-checked. If the compiler did not had a understanding of the types involved in actions it could not rewrite them without leaking memory and so on.

##### But is it portable?
The language is designed to behave the same independently from the underlying machine, furthermore since it is based on `LLVM` it is trivial to make it work on all machines where `LLVM` is supported(which is most).

##### But is it easy to integrate with other codebases?
`rlc`can emit `C` and `python` wrappers that make it easy to integrate with other programming languages. As a rule of thumb, if your code base can be integrated with `C`, it can be integrated with `rl`.

##### But did we really needed another language? / But couldn't this have been a macro/template/generic?
Unfortunately yes, various compiler-based and library-based solutions were attempted, but in the end, we reached the conclusion that a domain-specific language was needed. The exact technical nature of these reasons goes beyond the scope of this document, but intuitively the reason is that translating a "procedure-like" simulation into a "class-like" simulation is very similar to what a compiler does when it translates a structured function into a bunch of assembly branch instructions and collaboration from the typecheker is required. We have not found any language that was both fast enough and had the required metaprogramming facilities to implement this.

##### But what is the catch?
The main catch is that not all programs can be expressed trivially in `rl`.   `rl` is a domain-specific tool, and if your problem does not lie in that domain, `C` is still your best solution. Still, the language is designed to be lean and easy to integrate, so that the parts of your code base that make sense to be expressed in `rl` will do so, and the parts that don't, will not.

