# Language Tour

[Rulebook](https://github.com/rl-language/rlc) is a compiled and statically checked language.

```rlc
# rlc hellow world
import serialization.print

cls SimpleRegularCode:
    Int x
    Bool y

fun main() -> Int:
    let pair : SimpleRegularCode
    print(pair) # {x: 0, y: false}
    return 0
```

Its defining innovation is the concept of **action functions**—a mechanism designed to handle complex interactions—enhanced with **SPIN** properties. Rulebook carefully combines multiple uncommon languages features in a imperative language that is more than the sum of the parts.

## Action Functions

Rulebook is built to simplify the construction of [complex interactive subsystems](./the_inversion_of_control_problem.md#complex-interactive-subsystems). It achieves this through **SPIN functions**, which encapsulate stateful, inspectable logic without taking over control flow.

**SPIN** stands for:

* [Serializable](#serializable)
* [Precondition checkable](#precondition-checkable)
* [Inspectable](#inspectable)
* [No-main loop owning](#no-main-loop-ownership)

These properties allow developers to [store, load, print, replay, and modify](#spin-function-implications) both program state and execution traces in a principled and testable way.

---

## No-Main Loop Ownership

In interactive systems, programs must often wait for input from users or networks. Typically, such systems are governed by a **framework** that controls the main loop—such as a GUI or game engine—rendering frames while awaiting events.

Rulebook's **action functions** are coroutines. They don't need to control the main loop, making them inherently framework-agnostic.

Here’s an example. The `say_hello()` function runs until it hits an `act resume()` statement. From the main function, we resume execution using `sayer.resume()`, causing the next part of the coroutine to run:

```rlc
import serialization.print

act say_hello() -> HelloSayer:
    act resume()
    print("hello")
    act resume()
    print("hello")

fun main() -> Int:
    let sayer = say_hello()
    sayer.resume() # hello
    sayer.resume() # hello
    return 0
```

Thanks to this **no-main loop ownership**, you can write **interactive programs** as if they directly prompt the **user**, without needing to integrate deeply with or understand the details of the underlying **framework**.


## Precondition Checkable

**Precondition checkability** means the framework that owns the main loop can proactively validate user inputs—without embedding logic directly into the action function.

Consider this installer example: it prompts the user for a valid installation path, asks whether to include experimental content, and performs the installation based on that input.

```rlc
import string

act installer() -> Installer:
    show_path_screen_prompt()
    act select_location(frm String path) { !file_exists(path) }
    show_experimental_content_prompt()
    act install_experimental_content(Bool do_it)
    if do_it:
        install_with_extra(path)
    else:
        install(path)

fun main() -> Int:
    let installer = installer()
    let path = "wrong_path"s
    if can installer.select_location(path):
        installer.select_location(path)
        installer.install_experimental_content(False)
    return 0
```

In Rulebook, **suspension points** inside action functions are called **action statements**. These can take arguments and may include a **boolean condition** that defines when the action is valid. By using the `can` operator, the framework can check whether a given user input is valid **before** applying it—without complicating the action function with manual validation logic.

Beside `can` operator invocations, the spirit of Rulebook is to be used by other languages, so depending on your use case you configure how rulebook behaves when a precondition is not met but a function is called anyway.
* By default rulebook emits checks that invoke `rlc_abort`, a function that can be customized. The customization allows us for example to print a python stack trace when rulebook is used with full python interoperability. In our [4Hammer example](https://github.com/rl-language/4Hammer/blob/master/src/gdexample.cpp#L30) we customize for linux only the stack printing mechanism so we can see what is going on inside godot.
* If you need maximum speed, or you know that the caller will never invoke wrong actions by construction (for example,the c# wrapper checks for preconditions too and emits a exception if they are not met), you can disable checks from within rulebook code.

This makes precondition checkability a zero cost abstraction that you pay for only when you use it.

---

## Serializable

In many interactive systems, it’s useful—or even necessary—to **save and restore program state**. Rulebook supports this by allowing coroutines to be **serialized and copied**.

Let’s say you wrote a simple rock-paper-scissors game. After player 1 selects a move, you want to test every possible response from player 2 to find a winning one.

```rlc
enum Gesture:
    rock
    paper
    scizor

act play() -> Game:
    act select(frm Gesture player1)
    act select(frm Gesture player2)
    if player2_wins(player1, player2):
        print("you win")
    else:
        print("you lose")

fun main() -> Int:
    let state = play()
    state.select(Gesture::rock)

    let copy = state
    copy.select(Gesture::rock)   # you lose

    copy = state
    copy.select(Gesture::scizor) # you lose

    copy = state
    copy.select(Gesture::paper)  # you win
```

Because Rulebook supports serialization, you can **copy coroutine states**, explore different branches, and test all valid user interactions from any point. This makes techniques like automated testing, state-space exploration, and replay debugging straightforward and effective.

---

## Inspectable

Sometimes the framework needs access to internal coroutine state—such as a variable a user selected earlier—in order to render UI elements, track progress, or drive decision-making.

Revisiting the rock-paper-scissors example, suppose you want to read which move player 1 selected without executing the full game logic:

```rlc
enum Gesture:
    rock
    paper
    scizor

act play() -> Game:
    act select(frm Gesture player1)
    act select(frm Gesture player2)
    if player2_wins(player1, player2):
        print("you win")
    else:
        print("you lose")

fun main() -> Int:
    let state = play()
    state.select(Gesture::rock)
    print(state.player1) # rock
```

In Rulebook, any variable marked with the `frm` keyword is **automatically accessible** from outside the coroutine. This allows you to treat action functions as lightweight **stateful objects**—like classes. If a concept is naturally **procedural**, you can model it as a coroutine. If it’s naturally **structural**, you can define it as a class. Rulebook supports both approaches seamlessly.



## SPIN Function Implications

When used in an interpreted language or in combination with Rulebook’s [action statement classes](#action-statements-classes), **SPIN functions** unlock advanced techniques for managing **complex interactive systems**.

In addition to cleanly separating **UI framework concerns** from **application logic**, SPIN functions allow you to **store**, **load**, **print**, **replay**, and **modify** both the program’s state and its execution trace.

Here’s an example: this interactive program selects random actions, applies them to a state, stores the resulting actions in a trace, and later replays that trace on a new instance of the program.

```rlc
@classes
act installer() -> Installer:
    show_path_screen_prompt()
    act select_location(frm String path) { !file_exists(path) }
    show_experimental_content_prompt()
    act install_experimental_content(Bool do_it)
    if do_it:
        install_with_extra(path)
    else:
        install(path)

fun main() -> Int:
    let trace : Vector<AnyInstallerAction>
    let state = installer()
    while !state.is_done():
        let action = select_random_valid_action(state)
        apply(state, action)
        trace.append(state)

    state = installer()
    for action in trace:
        apply(state, action)

    return 0
```

If `select_random_valid_action` chooses valid actions, the trace might look like this:

```text
{ InstallerSelectLocation{ path: "some_valid_path" }, InstallerInstallExperimentalContent{ do_it: true } }
```

This technique is fundamental for:

* **Fuzz testing**
* **Reinforcement learning**
* **Behavioral verification**

By recording and replaying action sequences, you can rigorously test application logic **independently** of the surrounding infrastructure.

---

## Action Statement Classes

In interactive systems, you may want to **delay** the execution of an action—such as processing a user click after a few frames. Rulebook makes this possible using the `@classes` attribute on [action functions](#action-functions).

Here’s an example. A rendering loop polls GUI events, converts them into actions, queues them, and applies them with a delay:

```rlc
@classes
act installer() -> Installer:
    show_path_screen_prompt()
    act select_location(frm String path) { !file_exists(path) }
    show_experimental_content_prompt()
    act install_experimental_content(Bool do_it)
    if do_it:
        install_with_extra(path)
    else:
        install(path)

fun main() -> Int:
    let state : installer()
    let delayed_events : DelayedEventQueue
    let ui : GUI
    while true:
        render_frame()
        let events = poll_user_events()

        # Queue an event to be processed later
        if event.clicked_some_button:
            let action : InstallerSelectLocation
            action.path = ui.path_field.text
            delayed_events.append(action)

        if event.clicked_on_checkbox:
            let action : InstallerInstallExperimentalContent
            action.do_it = ui.checkbox.is_set
            delayed_events.append(action)

        # Apply events after a delay
        for event in delayed_events.get_events_to_apply():
            if can apply(event, state):
                apply(event, state)
```

When you annotate an action function with `@classes`, Rulebook generates a **class** for each action statement. These classes only contain the relevant arguments. Additionally, a **type-safe union**—`AnyInstallerAction`—is created to represent all possible actions.

In the example above, the generated classes would look like:

```rlc
cls InstallerSelectLocation:
    String location

cls InstallerInstallExperimentalContent:
    Bool do_it

using AnyInstallerAction = InstallerSelectLocation | InstallerInstallExperimentalContent
```

These classes can be **stored**, **serialized**, **printed**, **copied**, and—most importantly—**applied** using the built-in `apply` function.

By treating actions as first-class values, Rulebook allows you to **schedule and manage user interactions** without coupling your application logic to the GUI framework—encouraging cleaner, more modular architectures.


## Composing Actions

**Complex interactive subsystems**—the kind Rulebook is built to handle—often benefit from being broken down into **smaller, reusable parts**.

Rulebook enables this with a mechanism for composing action functions. Here’s an example of how to structure a compound interaction like rolling two dice, with the added ability to reroll based on the outcome:

```rlc
act roll_2_dice(Int num_faces) -> RollDice:
    act roll(frm Int first) {first > 0, first <= num_faces}
    act roll(frm Int second) {second > 0, second <= num_faces}

act rerollable_roll() -> RerollableDices:
    subaction* roll = roll_2_dice(6)
    if roll.first + roll.second == 2:
        roll = roll_2_dice(6)
        subaction* roll

fun main() -> Int:
    let sequence = rerollable_roll()
    sequence.roll(1)
    sequence.roll(1)
    sequence.roll(3)
    sequence.roll(4)
    print(sequence.roll.first)  # 3
    print(sequence.roll.second) # 4
```

The keyword `subaction*` exposes the inner action function's interface to the outside world—**until it completes**. This lets you structure your logic as **nested interactive sequences**, where each subsequence can be reused, composed, and tested in isolation.

Read more about `subaction*` [here](./language-reference.md#subaction-statements).

---

## Use Cases

Now that we've explored how [action functions](#action-functions) work, let’s look at real-world applications.

---

## Automatic Testing

Rulebook (RLC) includes a built-in **fuzzer** that can generate and apply random actions to your interactive programs—out of the box or customized for your needs.

> Curious about fuzzing? [Here's a brief intro.](https://en.wikipedia.org/wiki/Fuzzing)

Below is a simple example: an interactive sequence that collects user data. However, there's a bug—the program doesn't guard against invalid age values (e.g. zero or negative numbers):

```rlc
import action

@classes
act ask_user_data() -> AskUserData:
    act insert_nationality(frm Int nationality_id)
    act insert_age(frm Int age)
    if age > 18:
        return  # nothing else to do

    act insert_parent_nationality(frm Int parent_nation_id)
    if age < 0:
        assert(false, "age cannot be negative")

fun fuzz(Vector<Byte> input):
    if input.size() == 0:
        return
    let state = ask_user_data()
    let action : AnyAskUserDataAction
    let trace = parse_actions(action, input)
    for action in trace:
        if can apply(action, state):
            apply(action, state)
            print(action)
```

You can compile and run this test like so:

```bash
# On Linux
rlc file.rl -o executable --fuzzer
./executable                # Automatically crashes if invalid input is found
./executable crashing_trace.txt
# Output:
# ./file.rl:12:9 error: age cannot be negative
```

With minimal effort, you've created a **property-based test** that finds bugs by driving your interactive sequence with random—but valid—input.

In this case, the fix is simple: update the action statement with a guard condition:

```rlc
act insert_age(frm Int age) { age >= 0 }
```

---

### Why This Matters

Fuzzing is useful not only for **ensuring robustness**, but also for **rapid prototyping**. If you're iterating quickly on an interactive flow you might throw away tomorrow, don’t spend time writing manual tests—just run the fuzzer and confirm it doesn’t crash.

That said, most of a fuzzer’s time is spent generating and discarding invalid inputs. In production, you’ll often filter actions using `can apply`, so testing those rejections isn’t always necessary.

If your system allows for fully **enumerating all valid actions**—see [Finite Interactive Programs](#finite-interactive-programs)—you can write fuzzers that are far more efficient and targeted.

And depending on the design of your application, additional optimizations may be possible.



## Finite Interactive Programs

**Finite interactive programs** are systems in which the user can take only a **limited number of distinct actions**, even if the total number of possible combinations is large.

### Examples:

* **Chess**: While games can, in theory, go on indefinitely, at any given moment each player can only choose from a finite number of moves (limited by the number of pieces and legal board positions).
* **Mechanical vending machine**: The user can insert a fixed set of coins and choose from a predefined set of products.

Because of their finiteness, these systems are excellent candidates for classic algorithms such as [state space search](https://en.wikipedia.org/wiki/State_space_search).

### Example: Exhaustive State Testing

Here’s a Rulebook example that explores all valid states of a simple tic-tac-toe game using `enumerate`, and verifies that the program never crashes, regardless of the user’s input.

```rlc
@classes
act play() -> Game:
    frm board : Board
    while !board.full():
        act mark(BInt<0, 3> x, BInt<0, 3> y) {
            board.get(x.value, y.value) == 0
        }

        board.set(x.value, y.value, board.current_player())

        if board.three_in_a_line_player(board.current_player()):
            return
        board.next_turn()

fun main() -> Int:
    let frontier : Vector<Game>
    let any_action : AnyGameAction
    let actions = enumerate(any_action) # contains 9 elements
    frontier.append(play())
    while !frontier.empty():
        let state = frontier.pop()
        for action in actions:
            if can apply(action, state):
                frontier.append(state)
                apply(action, frontier.back())

    return 0
```

The core idea here is the use of `BInt<0, 3>`, which restricts inputs to integers between 0 and 2 (inclusive of 0, exclusive of 3). This constraint ensures that all `mark` actions are within valid board coordinates. In fact, this program **formally guarantees** that `play()` will never crash under any valid sequence of moves.

While this technique doesn’t scale to large programs due to the exponential growth of possible states, Rulebook’s **composable architecture** allows you to isolate and test individual interactive sequences—even if the entire system can't be exhaustively verified.

---

## Reinforcement Learning

If you install Rulebook via the Python package `rl_language`, you gain access to built-in support for [reinforcement learning](https://en.wikipedia.org/wiki/Reinforcement_learning).

Previously, we saw how to:

* [Prove](#finite-interactive-programs) that certain components never crash
* [Fuzz](#automatic-testing) larger programs to uncover failures

However, these methods focus on **correctness**—determining whether a program crashes or not. What if you're interested in **performance**, or how a particular metric evolves over time?

This is where **reinforcement learning** shines.

---

### Example: "Catch" — A Simple RL Environment

The following program implements **Catch**, a classic test environment in reinforcement learning. In this game, the player moves left or right to catch a falling ball. The score is 1 if the ball is caught; 0 otherwise.

The program is more detailed than previous examples because it includes:

* Number of players
* Current player logic
* Game state and scoring metrics

If the implementation is correct, a trained agent should eventually achieve a near-perfect average score.



```rlc
import serialization.print
import range
import collections.vector
import machine_learning
import action

# Constants for the game
const NUM_ROWS = 11
const NUM_COLUMS = 5

enum Direction:
    Left
    None
    Right

    fun equal(Direction other) -> Bool:
        return self.value == other.value

    fun not_equal(Direction other) -> Bool:
        return !(self.value == other.value)


using Column = BInt<0, NUM_COLUMS>
using Row = BInt<0, NUM_ROWS>

# The main Catch game
@classes
act play() -> Game:
    frm ball_row : Row
    frm ball_col : Column
    frm paddle_col : Column

    # Initialization - chance player selects starting ball column
    act set_start_location(Column col)
    ball_col.value = col.value
    paddle_col = NUM_COLUMS / 2

    # Game loop - player makes moves until ball reaches bottom
    while ball_row != NUM_ROWS - 1:
        act move(Direction direction)

        ball_row.value = ball_row.value + 1

        let actual_direction = direction.value - 1  # Convert to -1, 0, 1
        paddle_col = paddle_col.value + actual_direction

# Cell states
enum CellState:
    Empty
    Paddle
    Ball

    fun equal(CellState other) -> Bool:
        return self.value == other.value

    fun not_equal(CellState other) -> Bool:
        return !(self.value == other.value)

# Get cell state at specific row and column
fun cell_at(Game game, Int row, Int col) -> CellState:
    if row == NUM_ROWS - 1 and col == game.paddle_col.value:
        return CellState::Paddle
    else if row == game.ball_row.value and col == game.ball_col.value:
        return CellState::Ball
    return CellState::Empty



# Function for machine learning components to display the game state
fun pretty_print(Game game):
    # Generate string representation of the board
    let result = ""s
    for row in range(NUM_ROWS):
        for col in range(NUM_COLUMS):
            if cell_at(game, row, col) == CellState::Empty:
                result = result + "."s
            else if cell_at(game, row, col) == CellState::Paddle:
                result = result + "x"s
            else:
                result = result + "o"s
        result = result + "\n"s
    print(result)


# Return current player or special value if game is done
fun get_current_player(Game g) -> Int:
    if g.is_done():
        return -4  # Terminal state
    let column : Column
    if can g.set_start_location(column):
        return -1
    return 0  # Player 0 (the only player)

# Return score for ML training
fun score(Game g, Int player_id) -> Float:
    if !g.is_done():
        return 0.0
    if g.paddle_col.value == g.ball_col.value:
        return 1.0
    else:
        return 0.0

# Return number of players (always 1 for this game)
fun get_num_players() -> Int:
    return 1
```

The program is simply run with
```
rlc-learn file.rl -o network #ctrl+c to stop it after a while
rlc-play file.rk network # to see a game
```

The user never had to specify anything related to reinforcement learning. They simply defined the environment's rules—and everything else was inferred automatically.

The system learns to maximize the score based on how the game works. That score can be visualized later, making this approach valuable in two ways:

* To determine the **maximum achievable performance** of a program.
* To verify that the program behaves **as expected**—and that internal variables don't take on surprising or incorrect values.

---

## Self-Configuring UIs

One of Rulebook’s unique strengths is that it **exposes all [action types](#action-statement-classes)** available in an [action function](#action-functions). This allows UI code to:

* Inspect the current program state.
* Determine which actions are currently valid.
* Adapt dynamically to changes in application logic.

### Example: UI for Tic-Tac-Toe

Imagine you’re building a UI for a tic-tac-toe game. You’ve created 9 square components, one for each cell. Each square should:

* Flash when it can be clicked.
* Show its current content (empty, X, or O).
* Work correctly even if game rules change.

Using action classes, you can achieve all of that with minimal coupling:

```rlc
@classes
act play() -> Game:
    frm board : Board
    while !board.full():
        act mark(BInt<0, 3> x, BInt<0, 3> y) {
            board.get(x.value, y.value) == 0
        }

        board.set(x.value, y.value, board.current_player())

        if board.three_in_a_line_player(board.current_player()):
            return
        board.next_turn()

cls UISquare:
    Int x
    Int y

    def update(Game state):
        let action = GameMark
        action.x = x
        action.y = y
        if can apply(action, state):
            render_quad(glow, x, y)
        else if state.board.slots[x][y] == 0:
            render_quad(grey, x, y)
        else if state.board.slots[x][y] == 1:
            render_quad(blue, x, y)
        else:
            render_quad(red, x, y)

fun main() -> Int:
    let state = play()
    let ui_quads = make_quads()
    while true:
        for quad in ui_quads:
            quad.update(state)
        poll_and_apply_events(state)
    return 0
```

This UI is entirely driven by:

* The `board` data structure.
* The `mark` action and its `can` condition.

As long as these remain consistent, the UI will continue to function—even if the game rules or logic change behind the scenes. This makes Rulebook a natural fit for **self-configuring, resilient user interfaces**.

---

## Compatibility

Rulebook offers **bidirectional interoperability** with:

* C
* C++
* Python
* C#
* Godot

### Example: C# Interop

Here’s how a C# program might interact with a Rulebook coroutine:

```rlc
act play() -> Game:
    frm local_var = 0
    act pick(Int x)
    local_var = x
```

```c#
class Tester {
    public void test() {
        Game pair = RLC.play();
        pair.pick(3);
        return (int)(pair.local_var - 3);
    }
}
```

To take full advantage of Rulebook’s features, it’s recommended to use only **copyable and default-constructible types** inside Rulebook programs. If that’s not possible, you can use the `ctx` feature to inject context objects for handling non-copyable types outside the coroutine.

All advanced features in Rulebook are **opt-in**. If you don’t use them, you pay **no performance or complexity cost**. For example:

* You can apply [enumerate](#finite-interactive-programs) only to testable UI components, even if the full system is not finite.
* You can use reinforcement learning selectively, or avoid it altogether.
* You can rely on trace replay only in subsystems where time-invariance holds.

Rulebook is designed to **scale gracefully** based on the features you choose to use.

More on interoperability [here](./interoperating.md).

---

## Remote Execution

Since Rulebook supports [serialization](#serializable) for [action statement classes](#action-statement-classes) and is [interoperable](compatibility) across multiple languages, **remote execution** can be implemented at the **action trace level**.

That means:

* You can run identical programs in two separate processes (even across languages).
* As one instance executes actions, the trace can be sent to and replayed by the other.

This enables powerful distributed testing and simulation scenarios, and it's already used in the [4Hammer project](https://github.com/rl-language/4Hammer). It’s a natural extension of Rulebook’s [SPIN function](#spin-function-implications) architecture.

---

## Tooling

Rulebook includes a growing suite of development tools:

* **Autocomplete** for Visual Studio Code and any editor with LSP (Language Server Protocol) support.
* **Language bindings** with autocomplete for each supported language (e.g., C#, Python).
* **Basic GDB support**, though debugging is rarely needed due to the power of:

  * Trace replay
  * Fuzz testing
  * Serialization

The [4Hammer](https://github.com/rl-language/4Hammer) example was written **without a debugger**, relying solely on Rulebook’s core tooling. That said, if debugging support is important to you, let the team know—it’s an area they’re actively improving.

---

## Performance

Rulebook is designed for **near-native performance**, without sacrificing flexibility:

* **Action functions** are stack-allocated coroutines.
* Each one uses a single integer to track its resume point.
* **Structs** match the C ABI exactly.
* **Unions** are like C unions, with one added integer to indicate the active variant.
* **All arguments are passed by reference**, and return values are by value (unless marked `ref`).

The only performance trade-off comes from serialization:

* Rulebook uses **integer indices** instead of raw pointers, so all structures can be stored and restored safely.
* If absolute speed is needed, you can manually manage pointers and specify how they’re serialized/deserialized.

Importantly, **any other language offering equivalent serialization guarantees would face similar trade-offs**. Rulebook just makes the implications explicit.

We benchmark performance against C++ in the official language paper.

> *ToDo: Add link to the paper after de-anonymization.*

