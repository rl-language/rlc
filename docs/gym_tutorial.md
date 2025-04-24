# RLC and GYM


This document assumes the reader is already familiar with reinforcement learning and [GYM-style environments](https://gymnasium.farama.org). It is intended as a reference for those who wish to wrap their own Rulebook environments in a GYM-style wrapper to ensure interoperability with existing reinforcement learning algorithms.

It also assumes you are familiar with Rulebook. If not, we recommend starting with the [base tutorial](./tutorial.md).

In this document, you will learn:

- How to install `rlc-core`, a minimal pip package that depends only on NumPy and offers basic functionality.
- How to import an example program into Python.
- How to use `RLCSingleEnvironment` to access essential features when building your custom wrapper.

Specifically, we will cover:
- Handling tensor serialization, including custom serialization methods.
- Accessing the full list of actions and the subset of valid actions.
- Retrieving returns and rewards.
- Checking for the end of a game.
- Managing secret information and handling randomness.

Since there are too many GYM-like wrappers, we provide general functions that you can reuse to write the wrapper with the right interface for your usecase.


### Installing `rl_language_core`

This section assumes that you want to replace the default tools provided by RLC. The standard `rlc` pip package includes a dependency on PyTorch to allow users to quickly test their RLC programs. However, this setup may not be suitable if you plan to use a custom version of PyTorch or a different machine learning framework altogether.

To address this, we provide the `rl_language_core` package—a minimal alternative that includes only the essential components needed to run RLC programs. It also comes with supporting Python code that depends solely on NumPy.

To install it, run:

```
pip install rl_language_core
pip install numpy
```


### Example Program

Let’s begin with a simple RLC program and build on top of it. Below is an implementation of the classic game *rock-paper-scissors*. Note that this example does not define the number of players or specify how the game state maps to the current player—these details are left implicit.

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



### Loading the Program

Now that we have a Rulebook file, let's write a Python module to load it:

```python
# main.py

from rlc import compile

with compile(["./rockpaperscizzor.rl"]) as program:
```

This snippet compiles the `rockpaperscizzor.rl` file on the fly and returns a `Program` object—a wrapper that provides useful utilities like string conversions and formatted printing.

If you prefer not to use just-in-time (JIT) compilation—for example, if you don’t want to include the RLC compiler with your distribution—you can precompile the Rulebook program ahead of time and load it directly. *(TODO: Show how to do this.)*

However, our goal is not just to load the program and access its functions directly. Instead, we want to interact with it through a GYM-compatible interface. For that, we wrap it in a `SingleRLCEnvironment`.

```python
from rlc import compile
from ml.env import SingleRLCEnvironment, exit_on_invalid_env

with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
```


### Understanding `SingleRLCEnvironment`

`SingleRLCEnvironment` is a Python class that takes a compiled `Program` and exposes the helper functions typically expected from a reinforcement learning environment. It serves as a bridge between your Rulebook program and GYM-style interfaces.

For this wrapper to work correctly, certain information must be present and properly defined in the Rulebook program. To validate this, the function `exit_on_invalid_env` is provided. It will check the structure of your `Program` and exit with descriptive error messages if the expected types or functions are missing or incorrect.

Specifically, it checks that:

- A main action `play()` exists, and its coroutine return type is `Game`.
- A scoring function `score(Game game, Int player_id)` is defined and returns either a `float` or `int`.
- Every action's arguments implement the `Enumerable` trait. This trait is required to enumerate all possible actions and construct action tables.

Additionally, `exit_on_invalid_env` will emit warnings if the program contains objects meant to be used in training (e.g., tensors) but that lack a valid serialization strategy.

For example, consider this modification to the `play` action:

```python
@classes
act play() -> Game:
    act player1_move(frm Gesture g1)
    frm local_var : Int
    act player2_move(frm Gesture g2)
```

This will trigger a warning like the following:

```
WARNING: obj.local_var is of type Int, which is not tensorable. Replace it instead with a BInt with appropriate bounds, specify how to serialize it, or wrap it in a Hidden object. It will be ignored by machine learning.
```

These warnings help ensure that your environment is fully compatible with GPU-based training and other downstream machine learning tasks.


Very well, now that we have a valid environment, let’s take a closer look at what it contains.

A `SingleRLCEnvironment` includes:
* A game state, created by executing the user-defined `play` action.
* References to user-defined functions such as `score` and `get_current_player`.
* A table of all possible moves, allowing each move to be mapped to a unique integer.
* A copy of the current player’s score and the score received in the previous step.

### Dumping the State

You can print the current game state as follows:

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.print()
```

```
{resume_index: 1, g1: paper, g2: paper}
```

To obtain a tensor serialization of the state, use `get_state()`:

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print(env.get_state())
```

In this example, the serialized state is a one-hot representation:
- The first three floats represent the three possible values for player 1—rock, paper, and scissors.
- The next three floats do the same for player 2.
- The final float indicates the current player. Since we haven’t defined a function to determine the current player, this value defaults to zero.

```
[[[1.]]
 [[0.]]
 [[0.]]
 [[1.]]
 [[0.]]
 [[0.]]
 [[0.]]]
```
### All Actions

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print("Here are the game actions:")
    print([program.to_string(action) for action in env.actions()])
```

Running this program will print:

```
Here are the game actions:
['player1_move {g1: paper} ', 'player1_move {g1: rock} ', 'player1_move {g1: scizzor} ', 'player2_move {g2: paper} ', 'player2_move {g2: rock} ', 'player2_move {g2: scizzor} ']
```

### Valid Actions

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print("Here is a NumPy array indicating which actions are currently valid:")
    print(env.get_action_mask())
```

```
Here is a NumPy array indicating which actions are currently valid:
[1 1 1 0 0 0]
```

As expected, only the three actions available to player 0 are valid at the start of the game.

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

You can view the total score using:

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.step(1)
    env.step(5)
    env.print()
    print(env.current_score)
```

This prints an array where the *i*-th element represents the total score of player *i*. In the example, player 0 plays rock, player 1 plays scissors, and player 0 receives a score of 1.0:

```
{resume_index: -1, g1: rock, g2: scizzor}
[1.0]
```

You can also query the score obtained in the most recent step, instead of the cumulative score:

```python
with compile(["./rockpaperscizzor.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print(env.last_score)
```

```
[0.0]
```

### End of the Game

There are two ways to check whether the game has ended. The first is by using `is_done_underlying()`:

```python
with compile(["./file.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    env.step(1)
    env.step(5)
    print(env.is_done_underlying())
```

This checks whether the game has reached a terminal state. The output in this example is:

```
True
```

The second mechanism is discussed in the multiplayer section.

### Multiplayer

In your Rulebook code, you can enable multiplayer support by defining two additional functions: `get_num_players` and `get_current_player`. For example, add the following to your Rulebook file:

```python
fun get_num_players() -> Int:
    return 2

fun get_current_player(Game game) -> Int:
    if can game.player1_move(Gesture::rock):
        return 0
    return 1
```

You can get the ID of the player who needs to act next with:

```python
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.get_current_player())
env.step(1)
print(env.get_current_player())
```

This prints:

```
0
1
```

### Multiplayer End of Game

If we check the score in a multiplayer setup, we might notice something unexpected:

```python
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.current_score)
```

```
[1.0, 0.0]
```

We now see two values, one for each player. However, they don't sum to zero, which is unusual in a zero-sum game.

This behavior occurs because each player only observes the game from their own perspective. The score is updated for a player only after they take an action. To ensure that each player gets to observe the final state, the system automatically adds a fake final move for each one.

```python
env = SingleRLCEnvironment(program, solve_randomness=True)
print(env.current_score)
env.step(1)
print(env.current_score)
env.step(3)
print(env.current_score)
print(env.is_done_underlying())
print(env.is_done_for_everyone())
env.step(1)  # fake final action; the argument can be anything
print(env.current_score)
print(env.is_done_for_everyone())
```

`is_done_underlying` checks if the actual game logic has reached a terminal state.
`is_done_for_everyone` checks if all players have had the chance to observe the final state.
Only after `is_done_for_everyone` returns `True` will the scores reflect the full outcome of the game.

```
[0.0, 0.0]
[0.0, 0.0]
[0.0, -1.0]
True
False
[1.0, -1.0]
True
```

### Random Actions

You can specify that certain actions belong to a special player `-1`, which represents a random agent:

```python
fun get_num_players() -> Int:
    return 1

fun get_current_player(Game game) -> Int:
    if can game.player1_move(Gesture::rock):
        return 0
    return -1
```

When actions belong to player `-1`, they are automatically executed by sampling uniformly from the set of valid actions.

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
[1.0]  # The final result is random.
```

You can disable automatic execution of random actions by setting `solve_randomness=False`:

```python
env = SingleRLCEnvironment(program, solve_randomness=False)
```

In this case, you must manually check when a random action needs to be taken and handle it yourself.

### Hidden Information

To handle hidden information—data that should not be visible in the tensor serialization—you can use the `Hidden` type from the standard library.

Here's an updated version of the program using hidden actions:

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

Running the following:

```python
with compile(["./file.rl"]) as program:
    exit_on_invalid_env(program)
    env = SingleRLCEnvironment(program, solve_randomness=True)
    print(env.get_state())
```

will produce a serialized state where the player's actions are omitted, and only the current player ID is included.

`HiddenInformation` works similarly but allows you to specify which player is permitted to view the hidden data.

### Custom Tensor Representation

Your machine learning algorithms might require custom tensor representations different from the default one provided. You can define these by implementing two simple functions:

```python
fun write_in_observation_tensor(T obj, Int observer_id, Vector<Float> output, Int counter)
fun size_as_observation_tensor(T obj) -> Int
```

For example, to serialize the `Gesture` enum as a single float instead of a one-hot vector:

```python
fun write_in_observation_tensor(Gesture obj, Int observer_id, Vector<Float> output, Int counter):
    output[counter] = 2.0 * ((float(obj.value) / float(max(obj))) - 0.5)
    counter = counter + 1

fun size_as_observation_tensor(Gesture obj) -> Int:
    return 1
```

The line:

```python
output[counter] = 2.0 * ((float(obj.value) / float(max(obj))) - 0.5)
```

works as follows:
- `float(obj.value)` converts the enum's integer value to a float.
- Dividing by `float(max(obj))` scales the value to the [0, 1] range.
- Subtracting `0.5` recenters it to [-0.5, 0.5].
- Multiplying by `2.0` scales it to the [-1.0, 1.0] range.

This maps the three enum values to `-1.0`, `0.0`, and `1.0`.

When writing to the output vector, it is guaranteed that there is enough space, but you must manually update the `counter` to track how many floats you've written:

```python
counter = counter + 1
```

Finally, `size_as_observation_tensor` must return the maximum number of floats your type will ever write:

```python
fun size_as_observation_tensor(Gesture obj) -> Int:
    return 1
```
