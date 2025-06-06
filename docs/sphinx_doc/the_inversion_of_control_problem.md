# Interactive Inversion of control

When imperative [complex interactive subsystems](#complex-interactive-subsystems) are subjected to [inversion of control](https://en.wikipedia.org/wiki/Inversion_of_control) (**IOC**), they [become superlinearly harder](#the-mental-burden) to reason about, refactor, and maintain. Imperative languages provide low overhead [solutions](#mitigations-coroutines) to this problem, but when the complexity of the **interactive subsystems** increases, the probability of being able to use those features [decreases](#the-limits-of-coroutines).

Here are practical examples of real-world code that is hard to reason about, has inverted control, and is interactive.


* Blender’s stateful [user input event manager](https://github.com/blender/blender/blob/main/source/blender/windowmanager/intern/wm_event_system.cc). Because the event loop owns the timing of every mouse‑move, key‑press, and redraw, a small logic change (say, deferring key‑repeat) can subtly reorder callbacks. Reproducing a bug in similar **scenarios may require replaying** an entire input trace, and unit tests must mock dozens of event types to reach a single branch.
* Anaconda installer’s stateful [UI pages](https://github.com/rhinstaller/anaconda/blob/main/pyanaconda/ui/gui/spokes/storage.py). The installer UI pages (spokes) can appear in any order depending on earlier answers. Tests that assert “if option X then page Y collects field Z” must replicate many permutations; a new optional component multiplies the permutation space, inflating test suites and increasing the risk of missed edge cases.
* Google DeepMind’s [Hanabi](https://github.com/google-deepmind/hanabi-learning-environment/tree/master/hanabi_learning_environment). The game offers many possible player actions and must serialize state into tensors for neural‑network training. Performing local modifications to **data structures** can affect the surrounding classes, the tensor serialization code, and the order in which game actions execute—turning each change into a global problem.


## Complex interactive subsystems

**Complex interactive subsystems** are subsystems that:

* must await input from some other component of the system before deciding how to proceed, **and** the user cares about their content (**interactive**). The source of input does not have to be a human; it can be anything, including randomness.
* inputs change future states creating a graph of possible states, and the graph is large (**complex**).

Examples:

```{graphviz}
digraph CheckoutFlow {
    node [shape=box, style=filled, fillcolor=lightgray];

    Start -> CartReview;
    CartReview -> Login [label="Not Logged In"];
    CartReview -> AddressSelection [label="Already Logged In", style=dotted];

    Login -> AddressSelection;
    AddressSelection -> ShippingMethod;
    ShippingMethod -> PaymentMethod;
    PaymentMethod -> OrderReview;
    OrderReview -> ConfirmOrder;
    ConfirmOrder -> ReceiptPage;

    // Back edges
    Login -> CartReview [label="Back"];
    AddressSelection -> Login [label="Back"];
    ShippingMethod -> AddressSelection [label="Back"];
    PaymentMethod -> ShippingMethod [label="Back"];
    OrderReview -> PaymentMethod [label="Back"];
    ConfirmOrder -> OrderReview [label="Back"];

    // Error and conditional states
    AddressSelection -> InvalidAddress [label="Invalid", style=dashed];
    ShippingMethod -> ShippingUnavailable [label="Unavailable", style=dashed];
    PaymentMethod -> PaymentFailed [label="Invalid Card", style=dashed];
    ConfirmOrder -> OrderFailed [label="Payment Error", style=dashed];

    // Grouping
    subgraph cluster_errors {
        style=dashed;
        color=red;
        label="Errors / Exceptions";
        InvalidAddress;
        ShippingUnavailable;
        PaymentFailed;
        OrderFailed;
    }

    subgraph cluster_flow {
        label="Main Checkout Flow";
        color=black;
        Start;
        CartReview;
        Login;
        AddressSelection;
        ShippingMethod;
        PaymentMethod;
        OrderReview;
        ConfirmOrder;
        ReceiptPage;
    }
}

```

* **Webstore checkout page**: when a user clicks on the payment mechanism of a web page, the pages they are shown depend on what they filled in on previous pages. Some pages are unique to certain payment circuits. If the user has specified they live in a different country, additional data may be requested. The user is allowed to navigate back to previous pages.

* **Installer wizards**: the user's selection of components to install impacts the future pages they will be shown, as well as the additional data they are required to provide.

* **Online governmental forms**: the government does not care about your code quality. If they require a user to fill out 20 pages of forms where each page depends on the data entered in previous ones, that is how you must implement it.

* **Video games**: enemies in a first-person shooter have graph-like state machines describing states such as `running`, `shooting`, `falling`, driven by external events. Video games may contain hundreds of such state machines simultaneously, or individual machines with hundreds of states.

Experienced programmers know that graphs should be avoided when unnecessary — but sometimes, they are mandatory. The previous examples all illustrate situations where the business logic **requires** the use of graphs in the design of your solution.


## Inversion of control

The Wikipedia page on [inversion of control](https://en.wikipedia.org/wiki/Inversion_of_control) describes the concept in sufficient detail, so I suggest reading it before continuing. As a recap: **inversion of control** arises when a program takes over the main loop of execution and relegates the programmer to implementing callbacks, which the main loop owner invokes according to its internal logic.

This pattern is pervasive and intentional. Graphics engines, web engines, reinforcement learning environments, and more all use it to decouple engine internals from business logic implementation. Inversion of control is not an issue itself, it is a extremely useful tool to decouple the programming plumbings from the business logic.

## The problem

Let’s restate the problem:

* When **complex interactive subsystems** are subjected to [inversion of control](https://en.wikipedia.org/wiki/Inversion_of_control), they become superlinearly harder to reason about, refactor, and maintain.

This assertion is based on the observation that giving up control of the main loop in an **interactive system** (complex or not) push the program to be implemented as a [state machine](https://en.wikipedia.org/wiki/Automata-based_programming) or some equivalent construct, unless the programming language provides a mechanism to avoid it.

Let us consider an example: suppose we have a **interactive subsystem**, such as an implementation of Tic Tac Toe that owns the main loop. I will use python syntax, but what is relevant is not le language, but the need of writing is a state machine due to possible limitations with the language.

```python
def play():
    board = Board()
    while not board.full():
        (x, y) = query_some_input()
        board.set_cell(x, y, board.current_player)
        if board.three_in_a_line():
            return
        board.switch_current_player()
```


What happens if the language does not have facilities like coroutines or similar constructs (as is the case in `C`), and we must give up control of the main loop to, say, a graphical engine?

The only viable implementation would be a class that can be updated at will by the graphical engine whenever the user clicks on a cell.

```python
class TicTacToe:
  def __init__(self):
    self.board = Board()
    self.next_resumption_point = NormalTurn

  def update(self, x: int, y: int):
    self.board.set_cell(x, y, self.board.current_player)

    if self.board.three_in_a_line():
      self.next_resumption_point = Ended

  def is_done(self):
      return self.next_resumption_point == Ended

class Engine:
    ...
    def run():
      game = TicTacToe()
      while True:
          inputs = poll_input()
          if inputs.clicked_on_screen:
            game.update(inputs.x, inputs.y)
          render_frame(game.board)
```

This looked simple enough. Let us compose it into a more complex **interactive sequence**.

```python
# plays up to 2 times
def play_twice():
    game1 = TicTacToe()
    while not game1.is_done():
        (x, y) = user_input
        game1.update(x, y)

    play_again = user_input()
    if play_again:
        game2 = TicTacToe()
        while not game2.is_done():
            (x, y) = user_input
            game2.update(x, y)
```

The class version is:

```
class PlayTwice:
    def __init__(self):
        self.game1 = TicTacToe()
        self.game2 = None
        self.resume_index = Start

    def update(*args):
        if self.resume_index == Start:
            (x, y) = args
            self.game1.update(x, y)
            if self.game1.board.is_done():
                self.resume_index = Question
            return
        if self.resume_index == Question:
            (play_again) = args
            if play_again:
                self.resume_index = SecondGame
                self.game2 = TicTacToe()
            else:
                self.resume_index = Ended
            return
        if self.resume_index == SecondGame:
            (x, y) = args
            self.game2.update(x, y)
            if self.game2.board.is_done():
                self.resume_index = Ended
            return
```

As you can see, manually converting these functions into classes quickly results in very messy code.
As an exercise, convert the following piece of code into a class:

```python
# plays the game up to 4 times
def play_four_times():
    game1 = PlayTwice()
    while not game1.resume_index == Ended:
        game1.update(user_input)

    play_again = user_input()
    if play_again:
        game2 = PlayTwice()
        while not game2.resume_index == Ended:
            game2.update(user_input)
```

Hint, the graph representing the state is

```{graphviz}
digraph PlayFourTimes {
    node [shape=ellipse, style=filled, fillcolor=lightyellow];

    // States for first PlayTwice
    S0 [label="Start"];
    G1 [label="Play Game 1"];
    Q1 [label="Ask to Play Again?"];
    G2 [label="Play Game 2"];
    PT1_END [label="End of First PlayTwice"];

    // Transition to second PlayTwice
    Q2 [label="Ask to Play Again?"];
    G3 [label="Play Game 3"];
    Q3 [label="Ask to Play Again?"];
    G4 [label="Play Game 4"];
    END [label="Final End"];

    // First PlayTwice
    S0 -> G1;
    G1 -> Q1 [label="game1 complete"];
    Q1 -> G2 [label="yes"];
    Q1 -> PT1_END [label="no"];
    G2 -> PT1_END [label="game2 complete"];

    // After First PlayTwice
    PT1_END -> Q2;

    // Second PlayTwice (only if Q2 says yes)
    Q2 -> G3 [label="yes"];
    Q2 -> END [label="no"];

    G3 -> Q3 [label="game3 complete"];
    Q3 -> G4 [label="yes"];
    Q3 -> END [label="no"];
    G4 -> END [label="game4 complete"];
}
```

### The mental burden

I think it is intuitive to most programmers that this approach does not scale. Converting a function into a class is *syntactically* linear and follows a familiar “turn it into a state machine” recipe—but the real cost shows up later.

The **superlinearity** appears in maintenance. Each time you tweak the class‑based version, you must remember how the original imperative control flow was exploded into explicit states. A single new input or edge case can force you to revisit every branch that stores or resumes state. Concretely, if your program asks for **k** distinct inputs and you refactor it **r** times, the test‑case permutations can grow toward **k × r**—and that’s before you multiply by invalid or corner‑case inputs.

Sometimes **k** is small (e.g. Tic Tac Toe) or **r** is near 1 after initial authoring, so the pain stays minimal. But once either axis grows—say, a wizard with dozens of optional fields or a game with hundreds of enemy states—the maintenance burden balloons.

We are hardly the first to flag this. The issue is related—though not identical—to [callback hell](http://callbackhell.com) and the classic pitfalls of [non‑structured programming](https://en.wikipedia.org/wiki/Non-structured_programming), because steering a large explicit state machine feels like hand‑coding the program counter with jumps. Non imperative programming models can sometime help, if the domain of the business logic maps well onto them. [[1](https://en.wikipedia.org/wiki/Actor_model)] [[2](https://en.wikipedia.org/wiki/Functional_reactive_programming)] [[3](https://en.wikipedia.org/wiki/Structured_concurrency)]. Commenting on all of them is beyond the scope of the document, we will focus on the key imperative languages solution for the issue, coroutines.



## Mitigations: coroutines

The most common solution to the problem is the use of asynchronous language features.
In particular, [coroutines](https://en.wikipedia.org/wiki/Coroutine)—depending on their implementation—can sometimes fully solve the problem.

```python
def play(board):
  while not board.full():
    (x, y) = yield()
    board.set_cell(x, y, board.current_player)
    if board.three_in_a_line():
      return
    board.switch_current_player()


class Engine:
    ...
  def run():
    board = Board()
    game = play(board)
    while True:
      inputs = poll_input()
      if inputs.clicked_on_screen:
        game.send((inputs.x, inputs.y))
      render_frame(game.board)
```

Indeed, coroutines do exactly what we did by hand. They turned the imperative code into something that could be stopped and started according to the logic of the main loop holder.

## The limits of coroutines

Remember the second sentence in the original statement.
* Imperative languages provide low overhead [solutions](#mitigations-coroutines) to this problem (coroutines), but when the complexity of the **interactive subsystems** increases, the probability of being able to use those features decreases.

Complexity here is intrinsic to the business rules: no amount of engineering can remove the six screens in Amazon’s checkout if management requires them. That complexity therefore lies outside the programmer’s control.

Each new rule, branch, or state transition is another chance that some part of the flow will demand a capability absent from the coroutine model. Once a single branch cannot be expressed, you sometimes can keep half the program in coroutines and the rest elsewhere—sometimes you must rewrite the entire interaction as an explicit class‑based state machine.


**Example**:
You’ve implemented a user interface in Python using coroutines. It asks users for personal data—name, address, ID number, etc.—with the flow depending on their nationality.

Later, management introduces a new requirement:

> “At each screen transition, store the full application state so we can reload it later for debugging—*but without persisting any user data*, for privacy reasons.”

This seemingly small request breaks the coroutine-based design.

CPython coroutines are normally not serializable. You can’t snapshot the coroutine's execution state and restore it to be debugged elsewhere, and you can’t simply replay the coroutine from the beginning because you can’t store the sensitive user inputs. Without full serialization or replay, you lose the ability to recreate the interaction history—meaning the coroutine approach must be abandoned in favor of an explicit, inspectable, and serializable state machine.


### The core limitation


The core limitation of coroutines in imperative languages is:

> **If a coroutine implementation is not fully equivalent to a class (or the equivalent construct of the language) in all essential respects, then any business requirement that demands a feature supported by classes—but not coroutines—forces you to abandon the coroutine design, at least locally around the affected logic.**

Unfortunately, mainstream coroutine systems fall short. Most lack one or more capabilities that are standard in class-based designs. Here are some of the most common shortcomings (this list is not exhaustive):

* **No coroutine cloning** — Not many imperative languages support copying a live coroutine. If your interactive program stores critical state inside a coroutine and you need to fork it—say, to run parallel simulations as in [Monte Carlo Tree Search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search)—you’re out of luck. You must reimplement the logic using an explicit class that supports deep copying.

* **No safe introspection** — Many statically checked programming languages provide no portable way to inspect a coroutine’s internal state at runtime. Without access to that state, you cannot export it—for example, to a GPU in a reinforcement learning system—without resorting to fragile, non-portable hacks like leaking pointers or manually extracting values.

* **Limited composition and reuse** — Coroutines can often be composed and reused, but without delegating yields or similar constructs composing coroutines can become challenging.


In practice, coroutines are excellent tools for managing the *plumbing* of large systems. But when applied to **complex interactive subsystems**, the model starts to crumble. As program complexity grows, so too does the probability that you’ll need to **copy, inspect, serialize**, or some other class related feature, and the probability trends higher.


## Conclusion

The **interactive inversion of control problem** is not going away. Graphics engines, web frameworks, reinforcement learning platforms, and countless other systems suffer deeply from it. While modern languages offer coroutine-based workarounds, they are **fighting a losing battle**. To make real progress, we need language-level constructs designed to express interactive, stateful logic in a first-class, inspectable, and composable way.

[Rulebook](./language_tour.md#action-functions) solves this issue through first-class continuations, under the name of Action functions.

