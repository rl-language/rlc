# RLC

**Rulebook** is a language for complex interactive subsystems (reinforcement learning environments, videogames, UIs with graph-like transitions, multistep procedures, ...).

Rulebook is compiled and statically checked, the key and innovative feature of the language are [Action functions with SPIN properties](https://rl-language.github.io/language_tour.html#action-functions), which help to:

* [store, load, print, replay, modify](https://rl-language.github.io/language_tour.html#spin-functions-implications) both execution traces and the program state
* **automatically test** your interactive code using off-the-shelf [fuzzers](https://rl-language.github.io/language_tour.html#automatic-testing), [proofs](https://rl-language.github.io/language_tour.html#finite-interactive-programs) and [reinforcement learning](https://rl-language.github.io/language_tour.html#reinforcement-learning)
* write [self-configuring UIs](https://rl-language.github.io//language_tour.html#self-configuring-uis), where UIs can inspect the underlying program they present and configure themselves accordingly.
* [automatically remote execute](https://rl-language.github.io/language_tour.html#remote-execution) interactive code over the network.

Rulebook:

* **aids**, not replaces [C, C++, C#, Python, and Godot Script](https://rl-language.github.io/language_tour.html#compatibility)  (just like SQL aids but not replaces those languages)
* produces a single shared library (or webassembly if targeting the web) with the same ABI as C that you can embed in your software, wrapped into generated file native to your language.

Our key proof of concept example is [4Hammer](https://github.com/rl-language/4Hammer) . A never before implemented reinforcement learning environment with huge amounts of user actions in only ~5k lines of code (including graphical code). It runs in the browser and on desktop and all the features described in this section are present.

### Installation

Install rlc with:
```
pip install rl_language
```

If you don't want to use the off-the-self machine learning tools, you can install instead `rl_language_core` which has no dependencies beside numpy.

Create a file to test it is working, and fill it with the following content.
```
# file.rl

@classes
act play() -> Game:
    frm score = 0.0
    act win(Bool do_it)
    if do_it:
        score = 1.0
```

Then run with:

```
# On mac only run: export SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
rlc-learn file.rl --steps-per-env 100 -o net # ctrl+c to interrupt after a while
rlc-probs file.rl net
```
It will to learn pass true to `win` to maximize `score`, as reported by the second command.

##
### Documentation
[Project Rationale](./docs/where_we_are_going.md)

[Language Rationale](./docs/rationale.md)

[Tutorial](./docs/tutorial.md)

[Tutorial for GYM users](./docs/gym_tutorial.md)

[Paper for Reinforcement Learning users](https://arxiv.org/abs/2504.19625)

[Language reference and stdlib documentation](https://github.com/rl-language/rlc-stdlib-doc/tree/master)

### Contacts

[Discord](https://discord.gg/saSEj9PAt3)
[Twitter](https://twitter.com/RulebookL3873)
[Youtube](https://www.youtube.com/watch?v=tMnBo3TGIbU)
Or mail us at massimo.fioravanti@polimi.it



![RLC Logo](./imgs/RLC_logo.png)

### Example: tic tac toe
```

# declares the equivalent of a struct called Board.
# It contains the tic tac toe slots and the current player turn
# Methods omitted for brevity
cls Board:
	Int[9] slots
	Bool playerTurn

@classes
act play() -> TicTacToe:
	# allocates and initializes a board of type Board
	let board : Board
	while !full(board):

		# declares a suspension point of the simulation,
		# an action called mark that requires two ints to be performed.
		act mark(Int x, Int y) {
		# declares contraints about which inputs are valid
			x < 3,
			x >= 0,
			y < 3,
			y >= 0,
			board.get(x, y) == 0
		}

		# marks the board at the position provided
		board.set(x, y)

		# if the current player has three marks in a line
		# return
		if board.three_in_a_line():
			return

		board.change_current_player()


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

### FAQ:
#### I am a reinforcement learning engineer, what do I gain from using this?
By using RLC to write your environments, or to wrap previously existing environments, you obtain:
* the ability of automatically test those environments.
* configurable automatic serialization and deserialization textual and binary for those environments.
* configurable automatic serialization and deserialization textual and binary for sequences of actions instead of the state.
* configurable automatic serialization of the state to something that can be sent to the GPU for learning.
* the ability to reuse the environment code of the environment in production with no modification.

You can read more about the tutorial here [Tutorial](./docs/tutorial.md).

#### I am a graphic engine programmer/game programmer, what do I gain from using this?
By writing state and state evolution code (not graphical code) in Rulebook you obtain:
* the ability of automatically serialize the state to disk both in textual and binary form.
* the ability to automatically test and stress code witouth running the whole engine and thus testing it in isolation.
* the ability to reuse state code indipendetly from the engine.
* retain the ability of writing graphical code however you wish.

You can checkout a example where RLC is made interoperable with Godot [here](https://github.com/rl-language/4Hammer).

#### I can write the same tic tac toe example in python using python yields, what is the difference?
The difference is that when written in python:
* python coroutines lack a mechanism to express multiple possible resumption points.
* python coroutines allocate the coroutine state on the heap, RLC does not.
* you lose the ability to serialize and restore the execution of tic tac toe between player actions.
* you must use some special convention to extract the state of the board from the active coroutine, such as saving the reference to the board somewhere else.
* you must use special convention must be followed to express somewhere which values of x and y are valid and which are not, and such requirements cannot be expressed inline in the coroutine, defeating the advantage of using the coroutine.
* you must manually specify how to encode the suspended coroutine to something that can be delivered to machine learning components.

RLC does all of this automatically. You can read more about it [Here](./docs/rationale.md).


#### I have a previously existing code base, can I use this project?
Yes, at the moment Rulebook is compatible with python and C. You can use RLC as build only tool for testing purposes and not affect in any way your users.

#### I have performance constraints, is this fast?
We have performances comparable with C. Furthermore you can write C code and invoke it from Rulebook if you need ever more controll on performances.

#### In practice, what happens to a project that wants to include Rulebook components?
Everything about Rulebook will be turned into a single native library that you will link into or deploy along with your previously existing artifacts. Nothing else.


```
---------- 0 : p0 ------------
{resume_index: 1, score: 0.000000}
--------- probs --------------
0: win {do_it: true}  98.9385 %
1: win {do_it: false}  1.0615 %
------------------------------
{resume_index: -1, score: 1.000000}
```

Read a tutorial explaining how to play black jack [here](./docs/tutorial.md)

### License

We wish for `RLC` to be usable by all as a compiler, for both commercial and non-commercial purposes, so it is released under apache license.


## Info for compiler developers.
This section is dedicated to those that whish to build RLC itself from source, not to those that wish to use RLC as a off-the-shelf tool. At the moment we do not provide a off-the-shelf way of building RLC on windows due.

### Dependencies
Base:
* cpp17 compiler
* python
* CMake

Extra dependecies used by the setup script:
* Bash
* Ninja
* virtualenv
* lld


### Installation for compiler developers
Stop reading if you don't want to work on the compiler.

We provide a setup script that downloads the rlc repository and a setup script that will download and compile `LLVM` as well as `RLC`. As long as the dependencies written before are met you should just be able to run the following commands and everything should work. Installing and building llvm debug will take ~100 gigabytes of hard drive space and will require a large amount of time and RAM. This is only required when building from sources, pypi packages are less than 100mb on each OS.

Hard drive space can be reclaimed by deleting `LLVM` build directory after it has been fully built.

Download the setup.sh file in the root of the repository and then run:
```
chmod +x setup.sh
source ./setup.sh # clones RLC repo and initialize virtualenvs and submodules
python rlc/build.py # clones LLVM, builds it and builds RLC
```

on mac and windows replace the last line with
```
python rlc/build.py --no-use-lld
```

If that script terminates successfully, you are fully set up to start working on `RLC`.


#### Building the pip packages

You can create a pip package in BUILDDIR/dist/ by running
```
ninja pip_package
```

#### What do if run out of space or memory
Instead of the previous command python, you can run. This will only build the release LLVM version and save a great deal of space.
```
python rlc/build.py --no-debug-llvm
```

#### Using a custom LLVM
```
python rlc/build.py --llvm-dir <PATH-TO-LLVM-INSTALL> [--rlc-shared]
```

You need to use the flag --rlc-shared if you have built a shared LLVM.

### environment.sh
If you are using the default installation script (setup.sh) we provide a .sh file that configures your environment variable so that you can use python and rlc without installing anything in your actual machine.
When you open a shell to start working on RLC run the following command.

If you use some editor such as code or clion, start it from that shell.

```
source environment.sh
```

To check if everything works correctly run the following command.
```
python python/solve.py ./tool/rlc/test/tic_tac_toe.rl
```
If it does not crashes, then you are good to go.

If you use some whacky shell of your own or you did not followed the default setup, you are on your own.


### How to contribute for developers
* fork this project.
* push your branches to your fork
* open a pull request for the branch relevant to your project

The intent of this workflow is so that reviewrs can use the review feature of github pull requests to have persistent comment threads.

### Roadmap for 1.0

#### language
* better debug support
* inline initializers
* better cast guards

#### standard lib
* dictionary library


