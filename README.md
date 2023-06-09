# RLC

![RLC Logo](./imgs/RLC_logo.png)

The RuleBook Compiler (RLC) is a MLIR-based compiler for a domain specific language aimed at simplifying the complexity of developing multiagent simulations at all stages of development.

Given a RuleBook file that describes a simulation, it will be able to produce:
* a compiled library implementing such simulation
* a serialization and deserialization mechanism both in textual and binary format
* a network protocol able to run the simulation remotely
* a fuzzer able to find bugs in the simulation
* machine learning algorithms able to analyze the simulation.

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
		act mark(Int x, Int y)
		# declares contraints about which inputs are valid
		req x < 3
		req x >= 0
		req y < 3
		req y >= 0
		req board.get(x, y) == 0

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


### Installation for developers

We provide a setup script that will download and compile LLVM as well as RLC. As long as the dependencies written before are met you should just be able to download the setup.sh script and run it. Installing and building llvm will take ~150 gibabytes of hard drive space and will require a large ammount of time and RAM.

Hard drive space can be reclaimed by deleating LLVM build directory after it has been fully built.


```
chmod +x setup.sh
./setup.sh
```

If that script terminates successfully and prints "ALL DONE", you are fully set up to start working on RLC.

#### What do if run out of space or memory
You can edit the setup script so that the LLVM debug build is skipped and RLC cmake invocation uses the release LLVM build (notice, you have to replace both the LLVM and MLIR cmake arguments). That will make figuring out bugs in your code harder.

### environment.sh
If you are using the default installation script (setup.sh) we provide a .sh file that configures your environment variable so that you can use python and rlc without installing anything in your actuall machine.
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

