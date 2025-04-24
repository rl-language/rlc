# RLC and GYM

This document assumes the reader knows what reinforcement learning is, and what [GYM](https://gymnasium.farama.org) style environments are, and it intended to be a reference for those that with to wrap their own Rulebook environments into a GYM style wrapper for interoperability with already existing reinforcement learning algorithms.

This document expects you to know what Rulebook is, if you have not done so, consider reading instead the [base tutorial](./tutorial.md).

In this document you will see:
* How to install rlc core, the bare bone pip package that only depends from numpy and has barebone functionalities.
* How to import a example program into python.
* How to use RLCSingleEnvironment to get the basic functionalities you want to write your custom wrapper.

In particular we will see how to handle tensor serialization, including custom serializations, get the list of actions, the list of valid actions, the returns, the rewards, how to check for the end of the game, how to handle secret information and how to handle randomness.

### Installing rl_language_core

This document assumes you wish to replace the off-the-shelf tools RLC provides. For example, the default pip package for rlc comes with a dependency torward pytorch so that people can test their rlc programs. This is not acceptable if you want to use a custom pytorch or another machine learning framework.
For this reason, we provide rl_language_core, which only ships the minimal tools needed to run rlc programs and some python code that only depends on numpy.

You can install it with
```
pip install rl_language_core
pip install numpy
```

### Example program

Let us start with a very simple RLC program and then let us built on top of it. Here is a implementation of rock-paper-scizzor. Notice that the game does not specify the number of players, nor a mapping between game state and current player.

```python
# rockpaperscizzor.rl
enum Gesture:
    paper
    rock
    scizzor

@classes
act play() -> Game:
    act player1_move(frm Gesture g1)
    act player2_move(frm Gesture g2)

fun score(Game game, Int player_id) -> Float:
    if !game.is_done():
        return 0.0
    let winner = -1
    if game.g1.value == (game.g2.value + 1) % 3:
        winner = 0
    if game.g2.value == (game.g1.value + 1) % 3:
        winner = 1

    if winner == -1:
        return 0.0
    if winner == player_id:
        return 1.0
    return -1.0
```


### Loading program
Now that we have a file to load let us write a python module that can load it.

```python
# main.py

from rlc import compile

with compile(["./rockpaperscizzor.rl"]) as program:
```

This piece of code will compile on the fly rockpaperscizzor and gives you back `program: Program` which is a wrapper around to expose some usefull functions, such as convenient conversions to string or printing utilities. If you don't want to use a just in time approach, because for example you don't want to ship the rlc compiler to your users, you can instead ahead of time compile the Rulebook program and import it direclty. ToDo, show how.

In our case we do not simply with to load the program and access its functions directly, we want access it throught a GYM compatible point of view, for that reason we are going to wrap it into a SingleRLCEnvironment.

```python
from rlc import compile
from ml.env import SingleRLCEnvironment, exit_on_invalid_env

with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
```

SingleRLCEnvironment is a python class that given a Program, exposes all the helper functions you may want from a reinforcement learning environment.
Of course, for this to work, the relevant info must be present somewhere in the Rulebook program. To validate the correctness of the Rulebook program there is exit_on_invalid_env, which will print errors and exit if the the types and functions inside Program are not those to be expected.

In particular, it will check that:
* the main action `play()` exists and the implied coroutine type is called `Game`.
* `score(Game game, Int player_id)` exists and returns float or int.
* Every action is enumerable. That is, every argument of every action implements the trait Enumerable, which is use to build the table of all possible actions.

Furthermore it will emit warning if you have used objects that must be delivered to the GPU for training but that lack a conversion function.
For example, if you modify the `play`  action as follow
```python
@classes
act play() -> Game:
    act player1_move(frm Gesture g1)
    frm local_var : Int
    act player2_move(frm Gesture g2)
```

You will get a warning
```
WARNING: obj.local_var is of type Int, which is not tensorable. Replace it instead with a BInt with appropriate bounds or specify yourself how to serialize it, or wrap it in a Hidden object. It will be ignored by machine learning.
```

Very well, now that we have a valid env, let us discuss a bit what is inside of it.

A SingleRLCEnv contains:
* A game state obtained from executing the user defined `play` action function.
* Pointers to the users defined functions such as `score` and `get_current_player`.
* A table of all possible moves, so that each possible move can be associated to a single integer.
* A copy of the current player score, and of the score received at the last step.

### Dumping the state

You can print the state of the game as follow
```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.print()
```
```
{resume_index: 1, g1: paper, g2: paper}
```

You can obtain the tensor serialization of the state with `get_state()`.

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print(env.get_state())
```
In this example the serialized state is a 1-hot rappresentation where the first 3 float rappresent the 3 possible values of player 1 rock, paper and scizor, the second 3 are the same for player 2. The last float is reserved to specify who is the current player. Since we have not provided a function to specify the current player it will always be zero.
```
[[[1.]]

 [[0.]]

 [[0.]]

 [[1.]]

 [[0.]]

 [[0.]]

 [[0.]]]
```


### All actions
```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print("here are the game actions:")
    print([program.to_string(action) for action in env.actions()])
```

running this program will print
```
here are the game actions:
['player1_move {g1: paper} ', 'player1_move {g1: rock} ', 'player1_move {g1: scizzor} ', 'player2_move {g2: paper} ', 'player2_move {g2: rock} ', 'player2_move {g2: scizzor} ']
```

### Valid actions
```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print("Here is a numpy list of integers that tell you if a action is valid or not:")
    print(env.get_action_mask())
```
```
Here is a numpy list of integers that tell you if a action is valid or not:
[1 1 1 0 0 0]
```
as expected only the 3 actions that belong to player 0 are possible to execute at the start of the game.

### Applying actions

To advance the state, there is a very simple function, `step` which accepts the index of the action to execute and returns the reward of that action, measure as the different between the score before the action has been executed and the score after the action has been executed.
```
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.print()
    env.step(2) # 2 is the index in the list of actions you can access with env.actions()
    env.print()
```
```
{resume_index: 1, g1: paper, g2: paper}
{resume_index: 2, g1: scizzor, g2: paper}
```


### Score

You can see the total score with
```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.step(1)
    env.step(5)
    env.print()
    print(env.current_score)
```
which prints a array where each i-th element in the total score of player i. Youn can see it prints that player0 played rock, player1 played scizzors, and thus player0 returns are 1.0
```
{resume_index: -1, g1: rock, g2: scizzor}
[1.0]
```

You can as well query for the score value obtianed in the last step, instead of the global one.

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print(env.last_score)
```
```
[0.0]
```

### End of the game

There are two ways to checkin for the end of the game. The first is with `is_done_underlying()`
```python
with compile(["./file.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.step(1)
    env.step(5)
    print(env.is_done_underling())
```
It checks if the end of the game has been reached. Indeed the code snippet prints:
```
True
```

### Multyplayer

In your Rulebook code you can provide two extra functions to enable multiplayer, get_num_players and get_current_player. For example, add the following code to your rulebook file.

```python
fun get_num_players() -> Int:
    return 2

fun get_current_player(Game game) -> Int:
    if can game.player1_move(Gesture::rock):
        return 0
    return 1
```

You get the id of the player who has to act next with:

```python
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.get_current_player())
env.step(1)
print(env.get_current_player())
```
which prints integers
```
0
1
```

### Multiplayer end of game
If we try to observe the score, we get something strange.
```python
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.current_score)
```
we can see that there are now two values, but these two values do not add up to zero, which is strange in a zero sum game!

```
[1.0, 0.0]
```

This happens because we want to hide the actions played by other players. Each actor observes the game from their point of view, and it only gets to observe current state and rewards. For this reason the score of actors is only updated after they take a move, and a fake final move is added for each player, so that they can observe other players final moves.


```python
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.current_score)
env.step(1)
print(env.current_score)
env.step(3)
print(env.current_score)
print(env.is_done_underling())
print(env.is_done_for_everyone())
env.step(1) # fake final action, the argument can be anything
print(env.current_score)
print(env.is_done_for_everyone())
```

is_done_underlying tells you if the final state of the game has been reached. is_done_for_everyone tells you if every player had the possibility of observing the final state. After is_done_for_everyone returns true the score correctly sums to zero.
```
[0.0, 0.0]
[0.0, 0.0]
[0.0, -1.0]
True
False
[1.0, -1.0]
True
```

### Random actions
If you wish to, you can specify that some actions belong to player -1, that is, the random player.
```python
fun get_num_players() -> Int:
    return 1

fun get_current_player(Game game) -> Int:
    if can game.player1_move(Gesture::rock):
        return 0
    return -1
```

If you do so all actions that belong to that player will be performed at random by uniformly picking among the enumeration of valid actions.


```python
exit_on_invalid_env(program)
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.current_score)
env.step(1)
print(env.is_done_for_everyone())
print(env.current_score)
```
```
[0.0]
True
[1.0] # the final result is random of course.
```

As you probably notice, you can disable the automatic fast tracking of random actions by passing false to solve_randomness.
```python
env = SingleRLCEnvironment(program, solve_randomness=false)
```
If you do, you must check every time yourself if step must perform a random action.

### Hidden information

There is still one thing missing, hidden information. You can specify that some knowledge is not provided to the tensor serialization.
You can do in to ways, one is the standard library class `Hidden`

Here is the program rewritten to exploit hidden information

```python
import machine_learning

enum Gesture:
    paper
    rock
    scizzor

@classes
act play() -> Game:
    act player1_move(frm Hidden<Gesture> g1)
    act player2_move(frm Hidden<Gesture> g2)

fun score(Game game, Int player_id) -> Float:
    if !game.is_done():
        return 0.0
    let winner = -1
    if game.g1.value.value == (game.g2.value.value + 1) % 3:
        winner = 0
    if game.g2.value.value == (game.g1.value.value + 1) % 3:
        winner = 1

    if winner == -1:
        return 0.0
    if winner == player_id:
        return 1.0
    return -1.0


fun get_num_players() -> Int:
    return 1

fun get_current_player(Game game) -> Int:
    let x : Hidden<Gesture>
    if can game.player1_move(x):
        return 0
    return -1
```

if you now run the program
```
with compile(["./file.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print(env.get_state())
```

You will see that the player actions have been omitted from the serialized state, and not it only contains the current player id.

`HiddentInformation` works in the same way, except you can provide the ID of the player that is allowed to see the information

### Custom tensor rappresetation
Your machine learning algorithms may require different custom rappresentations than the off-the-shelf one we provide.
You can access them by providing two simple functions.
```python
    fun write_in_observation_tensor(T obj, Int observer_id, Vector<Float> output, Int counter)
    fun size_as_observation_tensor(T obj) -> Int
```

For example, immagine we want serilize the `Gesture` enum as a single float instead of a one-hot encoding

```python
fun write_in_observation_tensor(Gesture obj, Int observer_id, Vector<Float> output, Int counter):
    output[counter] = 2.0 * ((float(obj.value) / float(max(obj))) - 0.5)
    counter = counter + 1

fun size_as_observation_tensor(Gesture obj) -> Int:
    return 1
```

The line
```python
output[counter] = 2.0 * ((float(obj.value) / float(max(obj))) - 0.5)
```
`float(obj.value)` takes the integer value of the enum and turns into a float. `/ float(max(obj))` rescales that number to be between 0 and 1. `-0.5`
recenters the result to be between 0.5 and -0.5. Finally the `2.0` factor rescalases it between 1.0 and -1.0

In practice this remaps the 3 candidates of then enum into -1.0, 0.0, and 1.0.

When writing into the output vector it is always guaraneed that the vector can already contain your output, but you have to make sure that you keep track correctly of how much content you write.
```python
    counter = counter + 1
```

Finally, the function `size_as_observation_tensor` must return the maximal size the class can ever use, in number of floats.
```python
fun size_as_observation_tensor(Gesture obj) -> Int:
    return 1
```


