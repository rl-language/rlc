# RLC

![RLC Logo](./imgs/RLC_logo.png)
> ReLiC, the rlc dragon

The RuleBook Compiler (`RLC`) is an MLIR-based compiler for a domain-specific language aimed at simplifying the complexity of developing multiagent simulations at all stages of development.

The elevator pitch description of the `RL` is:
> **A language that turns a easy-to-write procedural description of a simulation into a easy-to-use and easy-to-reuse efficient library**.

Read the language rationale [here](./docs/rationale.md)

Given a RuleBook file that describes a simulation, it will be able to produce:
* a compiled library implementing such simulation (implemented)
* a serialization and deserialization mechanism both in textual and binary format (binary format implemented)
* a network protocol able to run the simulation remotely (development not yet started)
* a fuzzer able to find bugs in the simulation (development started)
* machine learning algorithms able to analyze the simulation. (development started)

### Example: tic tac toe
```

# declares the equivalent of a struct called Board.
# It contains the tic tac toe slots and the current player turn
# Methods omitted for brevity
ent Board:
	Int[9] slots
	Bool playerTurn

act play():
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

### License

We wish for `RLC` to be usable by all as a compiler, for both commercial and non-commercial purposes.
In the future, we would like to be able to distribute `RLC` entirely under Apache License, in a structure similar to `LLVM`, but that will depend on the finacial sustainability of the project.

At the moment `RLC` is distributed under the following licensing scheme.

* `RLC` standard library is released under Apache License V2, the intent is to let the standard library to be usable for any purpose, forever.
* `RLC` code is released under GPL2. As long as you do not redistribute the compiler and its libraries (which is not needed to use it as a compiler) you can use it for any purpose. This should include 95% of the use cases. If you redistribute them, for example, because you are developing a tool that uses the RLC parser library, you will have to distribute it under GPL2, or you will need to contact us for a custom license.

Should `RLC` ever include components that logically require to be linked to a user library to be useful, such as a just-in-time compiler library or runtime libraries, those libraries will have some exception from GPL2 and will be more permissive.


### Installation for developers

We provide a setup script that downloads the rlc repository and a setup script that will download and compile `LLVM` as well as `RLC`. As long as the dependencies written before are met you should just be able to run the following commands and everything should work. Installing and building llvm debug will take ~100 gigabytes of hard drive space and will require a large amount of time and RAM.

Hard drive space can be reclaimed by deleting `LLVM` build directory after it has been fully built.

Download the setup.sh file in the root of the repository and then run:
```
chmod +x setup.sh
source ./setup.sh # clones RLC repo and initialize virtualenvs and submodules
python rlc/build.py # clones LLVM, builds it and builds RLC
```

If that script terminates successfully, you are fully set up to start working on `RLC`.

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
python python/solve.py --source ./tool/rlc/test/tris.rl
```
If it does not crashes, then you are good to go.

If you use some whacky shell of your own or you did not followed the default setup, you are on your own.


### How to contruibute for developers
Do not push directly onto branches of this repo. Instead:
* fork this project. Make sure it is still private
* add me @drblallo to the developers for your fork
* push your branches to your fork
* open a pull request for the branch relevant to your project

The intent of this workflow is so that reviewrs can use the review feature of github pull requests to have persistent comment threads.


### Features  to develop next

#### missing for release
* allow declarations to be explicitly refs, in general require the user to specify this stuff and the emits error if its is wrong

* scalar type constraints for float

#### long term feature
* require clauses out of line (just allow to call any function i guess?)
* require clauses on structs
* inline initializers
* string support
* proper support for different architectures
* add "if x is Y as k:" to allow a way to dodge shadowing

#### standard lib
* graph library
* dictionary library
* random library

#### machine learning
* figure out how to make transformers fast
