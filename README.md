# RLC

![RLC Logo](./imgs/RLC_logo.png)
> ReLiC, the rlc dragon

### RL and RLC
The RuleBook Compiler (`RLC`) is an MLIR-based compiler for a domain-specific language aimed at simplifying the complexity of developing multiagent simulations at all stages of development.

The elevator pitch description of the `RL` is:
> **A language that turns a easy-to-write procedural description of a simulation into a easy-to-use and easy-to-reuse efficient library**.

Read the project rationale [here](./docs/where_we_are_going.md)
Read the language rationale [here](./docs/rationale.md)
Read how we analyzed a off the shelf game [here](./docs/space_hulk_level_design.md)
Read a tutorial explaining how to play black jack [here](./docs/tutorial.md)

At the moment `RLC` is a proof of concept, and is released to gather feedback on the features of the language. Until version 1.0 syntax and semantics may change at any point.

Before version 1.0 we want for users to be able to produce:
* a compiled library implementing such simulation (DONE)
* a serialization and deserialization mechanism both in textual and binary format (DONE)
* a fuzzer able to find bugs in the simulation (DONE)
* machine learning algorithms able to analyze the simulation. (development started)
* a simple network protocol able to run the simulation remotely (not yet started)

### Example: tic tac toe
```

# declares the equivalent of a struct called Board.
# It contains the tic tac toe slots and the current player turn
# Methods omitted for brevity
cls Board:
	Int[9] slots
	Bool playerTurn

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

### Try it!

At the moment we provide binaries only for linux x64.

```
pip install rl_language
```

Read a tutorial explaining how to play black jack [here](./docs/tutorial.md)

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

We wish for `RLC` to be usable by all as a compiler, for both commercial and non-commercial purposes, so it is released under apache license.


### Installation for developers

We provide a setup script that downloads the rlc repository and a setup script that will download and compile `LLVM` as well as `RLC`. As long as the dependencies written before are met you should just be able to run the following commands and everything should work. Installing and building llvm debug will take ~100 gigabytes of hard drive space and will require a large amount of time and RAM.
$$ E=\{\mathbb{I}\times\mathbb{I}\}\times B $$

But for simplicity we can just take it as $ E=\mathbb{I}\times\mathbb{I} $ <>
$$ E=\{\mathbb{I}\times\mathbb{I}\}\times B $$

But for simplicity we can just take it as $ E=\mathbb{I}\times\mathbb{I} $ <>

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
python python/solve.py ./tool/rlc/test/tic_tac_toe.rl
```
If it does not crashes, then you are good to go.

If you use some whacky shell of your own or you did not followed the default setup, you are on your own.

### Contacts

[Discord](https://discord.gg/saSEj9PAt3)
[Twitter](https://twitter.com/RulebookL3873)
[Youtube](https://www.youtube.com/watch?v=tMnBo3TGIbU)


### How to contruibute for developers
* fork this project.
* push your branches to your fork
* open a pull request for the branch relevant to your project

The intent of this workflow is so that reviewrs can use the review feature of github pull requests to have persistent comment threads.

### Roadmap

#### machine learning
* update to newer ray that does not leak [Blocked by them]
* update and pin to the newer ray that works on windows and does not have trivial issues [Blocked by them]
* figure out how to make transformers fast [Blocked by them]
* figure out how to not duplicate the state at every action and thus allow true self play [Blocked by them]

#### language
* better debug support
* inline initializers

#### standard lib
* graph library
* dictionary library

#### for 1.0
* windows support, blocked by ray

--------------------------------

# CURRENT FORK PROJECT: CONSTRAINT ANALYSIS

This branch is developed for implementing a specific compiler pass in MLIR for understanding the range of values that an argument passed to a RLC function can obtain.

## FOR ME:

Compile with : `--flattened --hide-position`

To run the passes (after doing `ninja all`) look at `tool/rlc-opt/rlc-opt`

Operations defined in `operations.td`

When pushing: `git push --set-upstream rlc_constraint_analysis constraint_analysis` 

## SOME INTRODUCTION ON THE FRAMEWORK AND ASSUMPTIONS

We have to develop a monotonic framework, so we need to define explicitly *backward/forward* - *lattice* - *join* - *transfer function* .

But before we start with that let's dive into some basic definitions and assumptions:

* at the end we expect that our program will output for each input argument a label of the form $( min , max )$ ( NB: in formal notation they should be squared brackets since the bounds are included but we are programmers not mere mathematicians )
* we can work under the assumption that $ min < max\quad\forall min,max \in \mathbb{I} $ , where $\mathbb{I}$ is our domain, indeed all the integer numbers between INT_MIN and INT_MAX ( we keep C++ notation here )
* notice that here we take INT_MIN and INT_MAX as the upper bounds of our domain ( infinity if you want )
* we can have that $min==max$ if we are **100% SURE** that a value is a constant
* for the moment we do not have the **overflow** of our variables, for a bit of reasons: 1) a pita to code 2) we hope that the developers will not purposely make variables overflow because they do not hate us

Now we are good to go and describe our particular analysys:

### BACKWARD/FORWARD

This is actually not a trivial idea and the solution to the problem comes after thinking a bit about the algorithm.

If you are thinking that a forward analysis is the correct one (like I did the first time), well you are pretty wrong I'm sorry.

The reason for this will become clear with an example later, for the moment just think that with a forward analysis if we perform an operation the new range we get is the one of the operation, so we can lose a bit of information on the initial variable unless we perform some backtracking.

### LATTICE 

As described in the assumptions above, we have a finite ( but decently big ) lattice of elements. To start we can define our interval as 

$$ \mathbb{I}=\{ x\in\mathbb{Z}:INT \textunderscore MIN\leq x \leq INT \textunderscore MAX\ \} $$

Where we remind that $INT \textunderscore MIN=-2^{31}$ and  $INT \textunderscore MAX=+2^{31}$ for 32 bit integer values.

Before describing the lattice we are missing another important piece, which is that any of our functions can either return true or false, so we define it simply as $B=\{0,1\}$ (or true/false, as you prefer).

Now our complete definition of the lattice is:

$$ E=\{\mathbb{I}\times\mathbb{I}\}\times B $$

But for simplicity we can just take it as $E=\mathbb{I}\times\mathbb{I}$ ( hopefully remembering that we have to keep track of both )

We can impose a partial ordering on it, but this will be done in the next section when we analyze the funny join function

NB: to be extremely precise the real lattice is a subset of the one described above since we work under the assumption that the left hand side (minimum) is less than the right hand side (maximum).

### JOIN

The join function is responsible for deciding which values should we consider when arriving from different paths of the program to a same node.

It needs to satisfy some useful properties, indeed it should be idempotent, commutative and associative. 

In our analysis is pretty straightforward to understand which is the join function.

Let's try to figure it out with an example. 
Suppose that my intervals to join are $(-5,8)$ and $(2,10)$. Then the minimums are -5 and 2 respectively. This means that from two different branches of our program ( they could be in a *for* loop for example ) we can have two different minimums. But by definition of a minimum it should be... the minimum, who whould have thougth!

We can do a similar reasoning with the maximum and arrive at the conclusion that in our example the join should return $(-5,10)$.

So we can say that in general:

For a pair of tuples $(a,b),(c,d)$ with $a,b,c,d \in \mathbb{I}$ we can define the join between them as:

$$ J((a,b),(c,d))=(\min(a,c),\max(b,d)) $$

Notice how this works also at the bounds of our domain.

Notice also how this creates a partial ordering relation, indeed for the minimum bound we have: $INT \textunderscore MIN>INT \textunderscore MIN+1>\cdots>0>1>\cdots INT \textunderscore MAX-1>INT \textunderscore MAX$ and for the maximum is reversed. 

Why? well because we define a partial ordering as $a\subseteq b \Longleftrightarrow J(a,b)=b$.

### TRANSFER FUNCTION

The idea behind the transfer function is to understand how the properties of our analysis change in the single basic blocks of the program.

Here in particular we need to analyze each single instruction that is derived after the **flattening** operation.

Now each operation acts as a possible transformation on our range, so it is better to keep each analysis separated and to start with a trivial example:

Consider the following pseudocode snippet:

```
fun trivial(int a):
	
	if(a > 0):

		temp1 = a + 2;
		if(temp1 < 10):

			return true;
		
		endif

	endif

	return false;

end
```

Now this is a simple program with a simple control-flow-graph (CFG). It is immediate to see that for the two returns the possible value of *a* is:

* $true: a\in(1,7)$
* $false: a\in(INT \textunderscore MIN,INT \textunderscore MAX)\setminus true $

Note that we want disjointed sets ( we can also overestimate if we want )

In order to understand how the algorithm should work we can fill a table of the form:

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |

Where we store at each step the information we found.

Now I will not draw the CFG of this problem since one can easily imagine it, so we can start with our backward analysis.

It should be indipendent which way we travel due to the properties of the join operation described above, so we can simply start by taking the path `if(a > 0) //false -> return false;`.

Here we know that we arrived from a false branch so the conditional `(a > 0)` has evaluated to false, which means that we can update our information as :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $(INT \textunderscore MIN,0)$| - |

We can take another branch for example `if(temp1 < 10) //false -> return false;`. Here things start to become a bit funky but we can still work with it. Now we can do the same reasoning as above and the new table becomes:

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $(INT \textunderscore MIN,0)$| $(10,INT \textunderscore MAX)$ |

We can continue going up this branch and encounter the operation `temp1 = a + 2` Which can be rewritten as `a = temp1 - 2` (NB: this for binary operations is a pretty easy transformation). Then if we are moving into false we can update our table but remaining very careful because we have already infromation about *a* in it, so we need to join:

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $J((INT \textunderscore MIN,0),(8,INT \textunderscore MAX-2))$| $(10,INT \textunderscore MAX)$ |

Now performing the join we can collapse the information as :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $(INT \textunderscore MIN,INT \textunderscore MAX -2)$| $(10,INT \textunderscore MAX)$ |

The only thing that we are left with in this branch is the fact that we arrive from `if(a > 0) //true`, so we should add to our table the information $(1,INT \textunderscore MAX)$ but we get the same, since it is a subset.

We are missing only one branch which is `if(temp1 < 10) //true -> return true;`. Now it should be pretty easy to compile the table since we understood how the algorithm works, so we start with

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | $(INT \textunderscore MIN,9)$ | $(INT \textunderscore MIN,INT \textunderscore MIN -2)$| $(10,INT \textunderscore MAX)$ |

Now we pass in `temp1 = a + 2` which gives a new value to *a* :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| $(INT \textunderscore MIN -2,7)$ | $(INT \textunderscore MIN,9)$ | $(INT \textunderscore MIN,INT \textunderscore MAX)$| $(10,INT \textunderscore MAX)$ |

And finally we know that we passed from `if(a > 0) //true` but remember, this is not a join but a transformation of our interval. So in the end the final table we compile is :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| $(1,7)$ | $(INT \textunderscore MIN,9)$ | $(INT \textunderscore MIN,INT \textunderscore MAX -2)$| $(10,INT \textunderscore MAX)$ |

At the end we can discard the temporary information and perform a disjunction of the sets to obtain the result above.

We can actually see that our final table gives us a bit more infromation, which is that if we for example decide to take *a* as INT_MAX-1 we will not get a meaningful answer ( probabily correct in this particular case ) because of overflow problems.

## OPERATIONS

We can start discussing which are the main operations treated in this analysis. For the moment i will subdivide them in :

* ARITHMETIC OPERATIONS ( + , * , - , /)
* BOOLEAN OPERATORS 
* ASSIGNMENTS 
* CONDITIONALS

Starting from the easiest :

### ADDITION

Now the addition is a pretty easy operator to resolve, since we can consider it as a shift of our range of values.

Then we can easily formalize the addition as:

$$ (a,b)+(c,d):(a+b,c+d) $$

Remembering that $a,b,c,d\in\mathbb{I}$.

NB: pay attention to the overflows

### MULTIPLICATION 

Multiplication is a bit more tricky but the general idea is that a multiplication by an integer number is a rescaling of my interval, with the only catch that if the number is negative we need to first invert it w.r.t. 0.

For example: $(-10,-3)\cdot(-2)=(3,10)\cdot2=(6,20)$.

Before presenting the general algorithm is interesting to formalize the multiplication by a scalar, so that we have it and do not forget it:

Assume $a,b,c\in\mathbb{I}$, then

$$ (a,b)\cdot c:(\min(a\cdot c,b\cdot c),\max(a\cdot c,b\cdot c)) $$

Then we can write a general algorithm, derived from all the possible transformations of the range:

```
(a,b)*(c,d):(min,max)

start

if b < 0 :
	if c > 0 :		//(-5,-3)*(3,5):(-25,-9)
		min = a * d
		max = b * c
	else if d < 0 :	//(-5,-3)*(-5,-3):(9,25)
		min = b * d
		max = a * c
	else :			//(-5,-3)*(-3,5):(-25,15)
		min = a * d 
		max = a * c
else if a > 0 : 
	if c > 0 :		//(3,5)*(3,5):(9,25)
		min = a * c
		max = b * d
	else if d < 0 :	//(3,5)*(-5,-3):(-25,-9)
		min = b * c
		max = a * d
	else :			//(3,5)*(-3,5):(-15,25)
		min = b * c
		max = b * d
else :
	if c > 0 :		//(-3,5)*(3,5):(-15,25)
		min = a * d
		max = b * d
	else if d < 0 :	//(-3,5)*(-5,-3):(-15,9)
		min = b * c
		max = a * d
	else : 			//(-3,5)*(-3,5):(-15,25)
		min = min( b * c , a * d )
		max = max( a * c , b * d )

end
```

I left also some comments with an example for each case.

NB: this can be rearranged in a more efficient way.

### SUBTRACTION

So, subtraction actually is not that different from addition, so we can basically use the same formula:

$$ (a,b)-(c,d):(a-b,c-d) $$

NB: the only thing to remember here is the non-commutativity

### DIVISION

Now i think division will be actually incredibly fun to do.

We have to remember that integer division ( at least in C and C++ ) truncates towards zero, this means that $5/2=2$ and $(-5)/2=-2$.

As the multiplication, the division is a rescaling of the range, but we have to be careful because we have a potential loss of information due to the truncation.

Before understanding the general case let's analyze also here the case for division by a constant:

Assume $a,b,c\in\mathbb{I}$, then

$$ (a,b)/c:(\min(a/c,b/c),\max(a/c,b/c)) $$

We can construct an general algorithm similar to the one above since the signs should be treated the same way, but with some small changes

```
(a,b)/(c,d):(min,max)

start

if b < 0 :
	if c > 0 :		//(-10,-3)/(3,10):(-3,0)
		min = a / c
		max = b / d
	else if d < 0 :	//(-10,-3)/(-10,-3):(0,3)
		min = b / c
		max = a / d
	else :			//(-10,-3)/(-3,10):(-10,10)
		min = a
		max = -a
else if a > 0 : 
	if c > 0 :		//(3,10)/(3,10):(0,3)
		min = a / d
		max = b / c
	else if d < 0 :	//(3,10)/(-10,-3):(-3,0)
		min = b / d
		max = a / c
	else :			//(3,10)/(-3,10):(-10,10)
		min = -b
		max = b
else :
	if c > 0 :		//(-3,10)/(3,10):(-1,3)
		min = a / c
		max = b / c
	else if d < 0 :	//(-3,10)/(-10,-3):(-3,1)
		min = b / d
		max = a / d
	else : 			//(-3,10)/(-3,10):(-3,10)
		min = a
		max = b

end
```

Here we can perform some interesting observations:

* there is a clear loss of information due to the truncation
* when the range (c,d) is discording this means that our range will include the numbers -1 and 1 => some cases can be simplified

NB: is this ok ?

### REMAINDER 

For the remainder I think we can perform some basic assumptions, indeed:

* we cannot perform the reminder with negative numbers as a base. If for some funky reason a programmer will try to do it he/she/it should be retroactively aborted (this is a citation to a famous programmer).

* can we perform the operation with a range of numbers ? Sure why not, but we still assume everything greater than zero.

Then under these assumptions the operation becomes actually quite simple, indeed:

Assume $a,b,c,d\in\mathbb{I}^{+}$, then

$$ (a,b)rem(c,d):(0,\max(b,d)) $$

With $\mathbb{I}^{+}=\{a\in\mathbb{I}:a\geq0\}$

Also here we notice an important loss of information that we need to cope with.

## ACTUAL ALGORITHM

Now everyone can do this kind of reasoning, but we need someone that integrates this analysis using a real tool (shame on you mathematicians! ). Aaaand here I am implementing it (or at least trying to).

More or less I have an idea of what the algorithm should do. Indeed a pseudo-pseudo code should be:

```
input : flattened_IR
output: flattened_IR_but_cooler 

algorithm:

	foreach (flat_function in flattened_IR):

		// ACTUAL ANALYSIS

		con = constraint_analysis(flat_function);

		// ATTACH TO THE NEW IR THE INFORMATION FOUND

		attach_arg_ranges_to_function(flat_function, con)


```

Now we encounter our first problem (this early? yes, such is the life of a programmer) : the analysis should be performed for each argument or for each function? (in mlir is the same as asking if we want a sparse or dense analysis) Because maybe they can be summed together and it is possible that i know the value of one before another. But if this is the case which argument should i try to evaluate first?

Sooooooo... what to do? Assuming that crying is not an option, we actually might have a solution. The solution is indeed hidden in the fact that we are performing a backward analysis! Let's assume that we have a case of `temp=a+b if(temp>0) return true else return false`, We do not know the range of both a and b, but we can say that if we return true we have $temp\in(0,INT \textunderscore MAX)$ and from that we can derive for both a and b that we have no information. Indeed the only information that we can pass along is that they are both non-negative and that the modulus of one is greater that the modulus of the other one. We still have no information on the constraints, they can be any number.

Then in the end... just using an analysis on the function should be enough.

Now be careful, the pseudo-pseudo-algorithm above is the algorithm the pass should perform because the analysis works on the dataflow, everything else works on the simple IR.

So now we have to define our lattice `mlir::rlc::ConstraintsLattice` and start from a `mlir::DenseBackwardDataFlowAnalysis`.

* windows support
* some very large hand written program with machine learning and all
* some other language testsuite transpiled to rl

--------------------------------

# CURRENT FORK PROJECT: CONSTRAINT ANALYSIS

This branch is developed for implementing a specific compiler pass in MLIR for understanding the range of values that an argument passed to a RLC function can obtain.

## FOR ME:

Compile with : `--flattened --hide-position`

To run the passes (after doing `ninja all`) look at `tool/rlc-opt/rlc-opt`

Operations defined in `operations.td`

When pushing: `git push --set-upstream rlc_constraint_analysis constraint_analysis` 

## SOME INTRODUCTION ON THE FRAMEWORK AND ASSUMPTIONS

We have to develop a monotonic framework, so we need to define explicitly *backward/forward* - *lattice* - *join* - *transfer function* .

But before we start with that let's dive into some basic definitions and assumptions:

* at the end we expect that our program will output for each input argument a label of the form $( min , max )$ ( NB: in formal notation they should be squared brackets since the bounds are included but we are programmers not mere mathematicians )
* we can work under the assumption that $ min < max\quad\forall min,max \in \mathbb{I} $ , where $\mathbb{I}$ is our domain, indeed all the integer numbers between INT_MIN and INT_MAX ( we keep C++ notation here )
* notice that here we take INT_MIN and INT_MAX as the upper bounds of our domain ( infinity if you want )
* we can have that $min==max$ if we are **100% SURE** that a value is a constant
* for the moment we do not have the **overflow** of our variables, for a bit of reasons: 1) a pita to code 2) we hope that the developers will not purposely make variables overflow because they do not hate us

Now we are good to go and describe our particular analysys:

### BACKWARD/FORWARD

This is actually not a trivial idea and the solution to the problem comes after thinking a bit about the algorithm.

If you are thinking that a forward analysis is the correct one (like I did the first time), well you are pretty wrong I'm sorry.

The reason for this will become clear with an example later, for the moment just think that with a forward analysis if we perform an operation the new range we get is the one of the operation, so we can lose a bit of information on the initial variable unless we perform some backtracking.

### LATTICE 

As described in the assumptions above, we have a finite ( but decently big ) lattice of elements. To start we can define our interval as 

$$ \mathbb{I}=\{x\in\mathbb{Z}:\textrm{INT\_MIN}\leq x \leq\textrm{INT\_MAX}\ \} $$

Where we remind that $\textrm{INT\_MIN}=-2^{31}$ and $\textrm{INT\_MAX}=+2^{31}$ for 32 bit integer values.

Before describing the lattice we are missing another important piece, which is that any of our functions can either return true or false, so we define it simply as $B=\{0,1\}$ (or true/false, as you prefer).

Now our complete definition of the lattice is:

$$ E=\{\mathbb{I}\times\mathbb{I}\}\times B $$

But for simplicity we can just take it as $ E=\mathbb{I}\times\mathbb{I} $ ( hopefully remembering that we have to keep track of both )

We can impose a partial ordering on it, but this will be done in the next section when we analyze a the funny join function

NB: to be extremely precise the real lattice is a subset of the one described above since we work under the assumption that the left hand side (minimum) is less than the right hand side (maximum).

### JOIN

The join function is responsible for deciding which values should we consider when arriving from different paths of the program to a same node.

It needs to satisfy some useful properties, indeed it should be idempotent, commutative and associative. 

In our analysis is pretty straightforward to understand which is the join function.

Let's try to figure it out with an example. 
Suppose that my intervals to join are $(-5,8)$ and $(2,10)$. Then the minimums are -5 and 2 respectively. This means that from two different branches of our program ( they could be in a *for* loop for example ) we can have two different minimums. But by definition of a minimum it should be... the minimum, who whould have thougth!

We can do a similar reasoning with the maximum and arrive at the conclusion that in our example the join should return $(-5,10)$.

So we can say that in general:

For a pair of tuples $(a,b),(c,d)$ with $a,b,c,d \in \mathbb{I}$ we can define the join between them as:

$$ J((a,b),(c,d))=(\min(a,c),\max(b,d)) $$

Notice how this works also at the bounds of our domain.

Notice also how this creates a partial ordering relation, indeed for the minimum bound we have: $\textrm{INT\_MIN}>\textrm{INT\_MIN}+1>\cdots>0>1>\cdots\textrm{INT\_MAX}-1>\textrm{INT\_MAX}$ and for the maximum is reversed. 

Why? well because we define a partial ordering as $a\subseteq b \Longleftrightarrow J(a,b)=b$.

### TRANSFER FUNCTION

The idea behind the transfer function is to understand how the properties of our analysis change in the single basic blocks of the program.

Here in particular we need to analyze each single instruction that is derived after the **flattening** operation.

Now each operation acts as a possible transformation on our range, so it is better to keep each analysis separated and to start with a trivial example:

Consider the following pseudocode snippet:

```
fun trivial(int a):
	
	if(a > 0):

		temp1 = a + 2;
		if(temp1 < 10):

			return true;
		
		endif

	endif

	return false;

end
```

Now this is a simple program with a simple control-flow-graph (CFG). It is immediate to see that for the two returns the possible value of *a* is:

* $true: a\in(1,7)$
* $false: a\in(\textrm{INT\_MIN},\textrm{INT\_MAX})\setminus true $

Note that we want disjointed sets ( we can also overestimate if we want )

In order to understand how the algorithm should work we can fill a table of the form:

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |

Where we store at each step the information we found.

Now I will not draw the CFG of this problem since one can easily imagine it, so we can start with our backward analysis.

It should be indipendent which way we travel due to the properties of the join operation described above, so we can simply start by taking the path `if(a > 0) //false -> return false;`.

Here we know that we arrived from a false branch so the conditional `(a > 0)` has evaluated to false, which means that we can update our information as :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $(\textrm{INT\_MIN},0)$| - |

We can take another branch for example `if(temp1 < 10) //false -> return false;`. Here things start to become a bit funky but we can still work with it. Now we can do the same reasoning as above and the new table becomes:

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $(\textrm{INT\_MIN},0)$| $(10,\textrm{INT\_MAX})$ |

We can continue going up this branch and encounter the operation `temp1 = a + 2` Which can be rewritten as `a = temp1 - 2` (NB: this for binary operations is a pretty easy transformation). Then if we are moving into false we can update our table but remaining very careful because we have already infromation about *a* in it, so we need to join:

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $J((\textrm{INT\_MIN},0),(8,\textrm{INT\_MAX-2}))$| $(10,\textrm{INT\_MAX})$ |

Now performing the join we can collapse the information as :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | -     | $(\textrm{INT\_MIN},\textrm{INT\_MAX-2})$| $(10,\textrm{INT\_MAX})$ |

The only thing that we are left with in this branch is the fact that we arrive from `if(a > 0) //true`, so we should add to our table the information $(1,\textrm{INT\_MAX})$ but we get the same, since it is a subset.

We are missing only one branch which is `if(temp1 < 10) //true -> return true;`. Now it should be pretty easy to compile the table since we understood how the algorithm works, so we start with

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| -    | $(\textrm{INT\_MIN},9)$ | $(\textrm{INT\_MIN},\textrm{INT\_MAX-2})$| $(10,\textrm{INT\_MAX})$ |

Now we pass in `temp1 = a + 2` which gives a new value to *a* :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| $(\textrm{INT\_MIN-2},7)$ | $(\textrm{INT\_MIN},9)$ | $(\textrm{INT\_MIN},\textrm{INT\_MAX-2})$| $(10,\textrm{INT\_MAX})$ |

And finally we know that we passed from `if(a > 0) //true` but remember, this is not a join but a transformation of our interval. So in the end the final table we compile is :

| TRUE |       | FALSE |       |
|------|-------|-------|-------|
| a    | temp1 | a     | temp1 |
| $(1,7)$ | $(\textrm{INT\_MIN},9)$ | $(\textrm{INT\_MIN},\textrm{INT\_MAX-2})$| $(10,\textrm{INT\_MAX})$ |

At the end we can discard the temporary information and perform a disjunction of the sets to obtain the result above.

We can actually see that our final table gives us a bit more infromation, which is that if we for example decide to take *a* as INT_MAX-1 we will not get a meaningful answer ( probabily correct in this particular case ) because of overflow problems.

## OPERATIONS

We can start discussing which are the main operations treated in this analysis. For the moment i will subdivide them in :

* ARITHMETIC OPERATIONS ( + , * , - , /)
* BOOLEAN OPERATORS 
* ASSIGNMENTS 
* CONDITIONALS

Starting from the easiest :

### ADDITION

Now the addition is a pretty easy operator to resolve, since we can consider it as a shift of our range of values.

Then we can easily formalize the addition as:

$$ (a,b)+(c,d):(a+b,c+d) $$

Remembering that $a,b,c,d\in\mathbb{I}$.

NB: pay attention to the overflows

### MULTIPLICATION 

Multiplication is a bit more tricky but the general idea is that a multiplication by an integer number is a rescaling of my interval, with the only catch that if the number is negative we need to first invert it w.r.t. 0.

For example: $(-10,-3)\cdot(-2)=(3,10)\cdot2=(6,20)$.

Before presenting the general algorithm is interesting to formalize the multiplication by a scalar, so that we have it and do not forget it:

Assume $a,b,c\in\mathbb{I}$, then

$$ (a,b)\cdot c:(\min(a\cdot c,b\cdot c),\max(a\cdot c,b\cdot c)) $$

Then we can write a general algorithm, derived from all the possible transformations of the range:

```
(a,b)*(c,d):(min,max)

start

if b < 0 :
	if c > 0 :		//(-5,-3)*(3,5):(-25,-9)
		min = a * d
		max = b * c
	else if d < 0 :	//(-5,-3)*(-5,-3):(9,25)
		min = b * d
		max = a * c
	else :			//(-5,-3)*(-3,5):(-25,15)
		min = a * d 
		max = a * c
else if a > 0 : 
	if c > 0 :		//(3,5)*(3,5):(9,25)
		min = a * c
		max = b * d
	else if d < 0 :	//(3,5)*(-5,-3):(-25,-9)
		min = b * c
		max = a * d
	else :			//(3,5)*(-3,5):(-15,25)
		min = b * c
		max = b * d
else :
	if c > 0 :		//(-3,5)*(3,5):(-15,25)
		min = a * d
		max = b * d
	else if d < 0 :	//(-3,5)*(-5,-3):(-15,9)
		min = b * c
		max = a * d
	else : 			//(-3,5)*(-3,5):(-15,25)
		min = min( b * c , a * d )
		max = max( a * c , b * d )

end
```

I left also some comments with an example for each case.

NB: this can be rearranged in a more efficient way.

### SUBTRACTION

So, subtraction actually is not that different from addition, so we can basically use the same formula:

$$ (a,b)-(c,d):(a-b,c-d) $$

NB: the only thing to remember here is the non-commutativity

### DIVISION

Now i think division will be actually incredibly fun to do.

We have to remember that integer division ( at least in C and C++ ) truncates towards zero, this means that $5/2=2$ and $(-5)/2=-2$.

As the multiplication, the division is a rescaling of the range, but we have to be careful because we have a potential loss of information due to the truncation.

Before understanding the general case let's analyze also here the case for division by a constant:

Assume $a,b,c\in\mathbb{I}$, then

$$ (a,b)/c:(\min(a/c,b/c),\max(a/c,b/c)) $$

We can construct an general algorithm similar to the one above since the signs should be treated the same way, but with some small changes

```
(a,b)/(c,d):(min,max)

start

if b < 0 :
	if c > 0 :		//(-10,-3)/(3,10):(-3,0)
		min = a / c
		max = b / d
	else if d < 0 :	//(-10,-3)/(-10,-3):(0,3)
		min = b / c
		max = a / d
	else :			//(-10,-3)/(-3,10):(-10,10)
		min = a
		max = -a
else if a > 0 : 
	if c > 0 :		//(3,10)/(3,10):(0,3)
		min = a / d
		max = b / c
	else if d < 0 :	//(3,10)/(-10,-3):(-3,0)
		min = b / d
		max = a / c
	else :			//(3,10)/(-3,10):(-10,10)
		min = -b
		max = b
else :
	if c > 0 :		//(-3,10)/(3,10):(-1,3)
		min = a / c
		max = b / c
	else if d < 0 :	//(-3,10)/(-10,-3):(-3,1)
		min = b / d
		max = a / d
	else : 			//(-3,10)/(-3,10):(-3,10)
		min = a
		max = b

end
```

Here we can perform some interesting observations:

* there is a clear loss of information due to the truncation
* when the range (c,d) is discording this means that our range will include the numbers -1 and 1 => some cases can be simplified

NB: is this ok ?

### REMAINDER 

For the remainder I think we can perform some basic assumptions, indeed:

* we cannot perform the reminder with negative numbers as a base. If for some funky reason a programmer will try to do it he/she/it should be retroactively aborted (this is a citation to a famous programmer).

* can we perform the operation with a range of numbers ? Sure why not, but we still assume everything greater than zero.

Then under these assumptions the operation becomes actually quite simple, indeed:

Assume $a,b,c,d\in\mathbb{I}^{+}$, then

$$ (a,b)\%(c,d):(0,\max(b,d)) $$

With $\mathbb{I}^{+}=\{a\in\mathbb{I}:a\geq0\}$

Also here we notice an important loss of information that we need to cope with.