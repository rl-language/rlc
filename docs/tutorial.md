# Rulebook Introduction

This documents aims at giving an overview of the RL language, providing a hands on approach were the reader may try the language on its own machine. By the end of this document you will learn to:

* Install the rl compiler using `pip`
* Learn how to write a program implementing `tic tac toe` and `rock paper scizzor`
* How run a machine learning algorithm that will learn to play both games

This document is intended to be a proof of concept, the language has not yet been released under version 1.0, and the our intent is to collect feedback about the language.

**This document requires some understanding of programming. If you can program in python or a similar language, you should be able to follow the content presented here.**

To be able to run the follwing examples you need:

* A linux x64 machine (we plan to support every major architecture and operative system in the future).
* At least `python 3.9` installation with `pip`

Optionally, you can install

* xdot, used to visualize the games
* visual studio code, for which we provide syntax highlighting and autocomplete plugins.

## Installation

You install the `rl` compiler, the autocomplete support and machine learnign dependencies by opening a terminal and running:
```bash
mkdir rl_example
cd rl_example
python3 -m virtualenv .venv
source ./venv/bin/activate
pip install rl_language
```

This command has created a virtual environment separated from your computer environment so that you can try `rlc` without polluting your machine.

If this command terminate with success, then you can validate that the installtion has been successfull with
```
rlc --help
```

Now we can write a very simple hello world program to see how it works.
Create a file called `example.rl` in the current directory, and fillit with the following content

```python
import serialization.print

fun main() -> Int:
    print("hello world")
    return 0
```

As you can see the language has a python-like syntax, although you can already notice that in RL types are not optional, they must be always present. We can now try compile the program and see it run.

```bash
rlc example.rl -o executable
./executable
```

The program should simply print `hello world`, and then terminate.

## Implementing Rock Paper Scizzor

Rock paper scizor is among the simplest games that can be implemented, and we wish to use it to introduce you to the peculiarities of RL.
Before implementing the rules, we need to declare a enum to rappresent player choises.
Add to the file the following lines

```python
enum HandType:
    rock
    paper
    scizzor

    fun wins_over(HadType other) -> Bool:
        if self == HandType::rock and other == HandType::scizzor:
            return true
        if self == HandType::paper and other == HandType::rock:
            return true
        if self == HandType::scizzor and other == HandType::paper:
            return true
        return false
```

Here we can see how `enums` work in `RL`, they are very similar to `C` or `python` enums, except you are allowed to declare methods inside them. Furthermore we can se that boolean types are called `Bool` and their value is accessible with the true and false keywords.

Now let us write the main function of the game:

```python
act play() -> RockPaperScizzorGame:
    while true:
        act throw_hand_p1(frm HandType p1_choise)
        act throw_hand_p2(frm HandType p2_choise)
        frm winner : BInt<-1, 2>
        if p1_choise.wins_over(p2_choise):
            winner.value = 0
            return
        if p2_choise.wins_over(p1_choise):
            winner.value = 1
            return

fun main() -> Int:
    let state = play()

    state.throw_hand_p1(HandType::rock)
    state.throw_hand_p2(HandType::scizzor)

    print(state)
    return state.winner.value
```

then we can compile once again the same way as before, from the same terminal as before

```bash
rlc example.rl -o excutable
./executable
```

This function we have just written may look very intimidating, indeed it contains constructs you probably have never seen elsewhere.
Let go thourh it line by line

### Action Function
```
act play() -> RockPaperScizzorGame:
```
the first line declares a `action function` called play, that when invoked initializes a game of type `RockPaperScizzorGame`. `Action functions` are **very** different from regular functions. When normal functions are executed they start from the top and do not stop executing until a `return` statement is encountered. `Action function` may instead be supsended and resumed, and the leverage this property to express clearly the most important characteristics of game, that is, player actions.


```
while true:
```

`RL` is a imperative language, so it shares constructs such as `if` and `while` with other imperative language, like `python` and `c`. Nothing suspicious is going on in this line.

### Action Statements
```
act throw_hand_pX(frm HandType p1_choise)
```

Here is another line prefixed with act. For the moment, ignore the keyword `frm`, we will explain later what it means. Before `act` meant that the function we were declaring is not a regular function but rather a `action function`. Here it means something different. It is a `action statement` and it means that we are introducing a `player action` called `throw_hand_pX`. When a `action statemetn` is encountered in the execution of a `action function`, the execution of the function is interrupted until the caller of the `action fuction` does not invoke the `action statement`.

Consider the `main` function
```python
let state = play()

state.throw_hand_p1(HandType::rock)
state.throw_hand_p2(HandType::scizzor)
```

As we just indicated, the `main` function invokes the `action function` named `play` and assigns it to the variable state. Since `play` contains some `action statements` called `throw_hand_pX`, the `play` function is not fully executed. It is interrupted when the first `act throw_hand_pX` is encoutered, then the computation resumes in the main function, where `state.throw_hand_pX(HandType::rock)` is executed next. At that moment, the computation resumes within `play`, from the location where it was suspended, and the argument of the function call (`HandType::rock`) is bounded to the argument of the `action statement` (`p1_choise`).

Take a moment to reason about you have just read, consider trying add, remove or change lines of code from main to better understand what is happening.

### Frame variable

```python
frm winner : BInt<-1, 2>
```

We already saw how to declare a variable in `RL`, in the `main` function we used the keyword let. A variable declared with `let` is a local variable and can only be accessed from within the function using it. Variable introduced by the keyword `frm` are instead `frame variables`. `Frame variables` are accessible both from within the `action function` they are declared and from the variable obtained from invoking the `action function` they belong to. In this case it means that `winner` is accessible from the `main` function, which we do access with

```python
return state.winner.value
```

You can try and replace `frm` with `let` and you will see that `winner` is no longer accessible from within main.

In some situations variables must be `frame variables`, this happens when the variable is needed between two different actions, and thus must be saved somewhere while the `action function` is suspended. In those situations the compiler will alert you that variables must be marked with the correct keyword.

The `: BInt<-1, 2>` simply entails that the type of the variable is `BInt`, a type that rapresents a integer bounded by a min and a max, in this case -1 and 2. This way we can rapresent the absence of winner with -1, player 1 as the winner with 0, and player 2 with 1. We will see later, when discussing the machine learning components, why being precise about the bounds of variables is important.

## Machine learning

We saw how to setup a very simple and very compact Rock Paper Scizzor game. Let us now investigate how we can train a reinforcement learning model that plays it.

First of all, we need to let the machine learning know a couple of info. We need to let know the machine learning how many players are in the game, and who is the current player given a certain state of the game.

```python
fun get_player_count() -> Int:
    return 2

fun get_current_player(RockPaperScizzorGame game) -> Int:
    if game.is_done():
        return -1
    let hand : HandType
    if can game.throw_hand_p1(hand):
        return 0
    return 1
```

Here we can see other interesting feature of the `RL` language.

`Action functions` automatically create a `is_done` member function, that will return true if and only if the game it has been invoked on has reached the end of its life.

Furthermore, you can check if a function can be invoked with the `can` keyword. `can game.throw_hand_p1(hand)` will return false unless `throw_hand_p1` is the next action to be executed in the game.

We need as well anther function, one that give the state of the game, and the current player returns the score of that player.

```python
fun score(RockPaperScizzorGame game, Int current_player) -> Float:
    if !game.is_done():
        return 0.0
    if game.winner.value == current_player:
        return 1.0
    return -1.0
```

We raccomend for scores to be ranged from -10 to 10, and not beyond that.

We need some suggestions about how long will the game take as well. The two players drawing all the way to eternity, but for the moment let us assume they will end the game in less than 10 attempts.

```python
fun max_game_lenght() -> Int:
    return 20 # 10 attempts * 1 move per player * 2 players
```

You may have noticed that at this point we have not specified anywhere that `throw_hand` are simultaneus actions, we will see later how to do so.


Finally, we need to add the following lines of code, this will generate some code needed by python to be able to inspect the game. Just copy paste this code and ignore.

```
fun gen_helper_methods():
    let state : RockPaperScizzorGame
    let any_action :  AnyRockPaperScizzorGameAction
    gen_python_methods(state, any_action)
```

### Learning

Ok, now we just need to run the learning algorithm, from the terminal where you run `rlc`, run instead:

```
rlc-learn example.rl -o network
```

You will see the command printing plenty of informations
[ToDo]


Now that the network has learned we can see how it plays. You can run the following commands to generate and see a game

```bash
rlc-play example.rl network -o trace.txt
cat trace.txt
```

it should print something like

```
throw_hand_p1 {rock}
throw_hand_p2 {paper}
```

You can run it multiple times, each time you should see that player 2 always wins the game. How is that possible? As we mentioned before the issue is that we have not specified that the choise made by player1 is a secret information until the second player operates their own choise. Let us change the code to reflect that.

```
act play() -> RockPaperScizzorGame:
    while true:
        act throw_hand_p1(HandType p1_choise)
        frm p1_selection : HiddenInformation<HandType>
        p1_selection.owner = 0
        p1_selection.value = p1_choise
        act throw_hand_p2(HandType p2_choise)
        frm winner : BInt<-1, 2>
        if p1_choise.value.wins_over(p2_choise):
            winner.value = 0
            return
        if p2_choise.wins_over(p1_choise.value):
            winner.value = 1
            return

```

What did we do here? We removed the `frm` keyword from the two `action statemets`, this way `p1_choise` and `p2_choise` are demoted to local variables, and are not visibile to the machine learning serialization. Then we created a new variable, called `p1_selection` of type `HiddenInformation<HandType>`, this is just a wrapper around a `HandType` except it contains as well a integer that rapresents who is the owner of that information. Only the player with the same ID as the owner will have access to that information during training.

Ok, let us run again the training step as before.

```
rlc-learn example.rl -o network

rlc-play example.rl network -o trace.txt
cat trace.txt
```

You should now see that the game is won on average 33% of the times by both players.

## Making more complex games

We have seen how to write a very simple game, and how to train a network to play it. Before letting you go, let us see how *compose*, composition is a core feature or `RL`, it allow us to reuse code and to test multiple configurations given the same code.

Rename the `play` function to `play_rock_paper_scizzor`

Add to your file the following code, this is a implementation of `TicTacToe`

```
ent Board:
    Int[9] slots
    Bool playerTurn


    fun get(Int x, Int y) -> Int:
        return self.slots[x + (y*3)]

    fun set(Int x, Int y, Int val):
        self.slots[x + (y * 3)] = val

    fun full() -> Bool:
        let x = 0

        while x < 3:
            let y = 0
            while y < 3:
                if self.get(x, y) == 0:
                    return false
                y = y + 1
            x = x + 1

        return true

    fun three_in_a_line_player_row(Int player_id, Int row) -> Bool:
        return self.get(0, row) == self.get(1, row) and self.get(0, row) == self.get(2, row) and self.get(0, row) == player_id

    fun three_in_a_line_player(Int player_id) -> Bool:
        let x = 0
        while x < 3:
            if self.get(x, 0) == self.get(x, 1) and self.get(x, 0) == self.get(x, 2) and self.get(x, 0) == player_id:
                return true

            if self.three_in_a_line_player_row(player_id, x):
                return true
            x = x + 1

        if self.get(0, 0) == self.get(1, 1) and self.get(0, 0) == self.get(2, 2) and self.get(0, 0) == player_id:
            return true

        if self.get(0, 2) == self.get(1, 1) and self.get(0, 2) == self.get(2, 0) and self.get(0, 2) == player_id:
            return true

        return false

    fun current_player() -> Int:
        return int(self.playerTurn) + 1

    fun next_turn():
        self.playerTurn = !self.playerTurn

act play_tic_tac_toe() -> TicTacToe:
    frm board : Board
    while !board.full():
        act mark(BInt<0, 3> x, BInt<0, 3> y) {
            board.get(x, y) == 0
        }

        board.set(x.value, y.value, board.current_player())

        if board.three_in_a_line_player(board.current_player()):
            return

        board.next_turn()

fun gen_printer_parser_tic_tac_toe():
    let state : TicTacToe
    let any_action :  AnyTicTacToeAction
    gen_python_methods(state, any_action)
```

Take your time to understand what is happening, you have all the knowledge you need to do so. If you wish you can rename `play_tic_tac_toe` to `play` and train a network to play it.

## Subaction statements

As we mentioned, the final step of this tutorial is composition, we will see how to compose rock paper scizzor and tic tac toe in two ways. One of them forces the players to play one of the two games at random, the other makes them play both games one at the time.

First let us see how to play one of the two games at random. Replace the helper functions with those with the same name reported here

```python
act play() -> ComposedGame:
    act random_choise(frm Bool is_tic_tac_toe)
    if is_tic_tac_toe:
       subaction* tic_tac_toe = play_tic_tac_toe()
    else:
       subaction* rock_paper_scizzor = play_rock_paper_scizzor()

fun gen_printer_parser():
    let state : ComposedGame
    let any_action :  AnyComposedGameAction
    gen_python_methods(state, any_action)

fun score(RockPaperScizzorGame game, Int current_player) -> Float:
    if !game.is_done():
        return 0.0
    if game.is_tic_tac_toe:
        return float(game.tic_tac_toe.board.three_in_a_line_player(current_player))
    if game.rock_paper_scizzor.winner.value == current_player:
        return 1.0
    return -1.0

fun get_current_player(RockPaperScizzorGame game) -> Int:
    if game.is_done():
        return -1
    if can game.random_choise(true):
        return -4
    let hand : HandType
    if can game.throw_hand_p1(hand):
        return 0
    if can game.throw_hand_p2(hand):
        return 1
    return game.tic_tac_toe.board.current_player()
```

`subaction*` is a new construct, it means that the `action function` on the right side operand of the `=` must be executed entirelly before terminating the `subaction statement`.

We had to change as well the `score` function and the `get_current_player` function. The remarkable line here is `return -4` in `get_current_player` which singifies that no player must act in that situation, instead the result is selected at random.

You can now train the game, and you should see that the generated games play half of the times tic tac toe and half of the times rock paper scizzors, both of which should be played perfectly.

Finally, let us see how to compose games in another way.

```python
act play() -> ComposedGame:
    frm tic_tac_toe = play_tic_tac_toe()
    frm rock_paper_scizzor = play_rock_paper_scizzor()
    while !tic_tac_toe.is_done() and !rock_paper_scizzor.is_done():
        if !tic_tac_toe.is_done():
            subaction tic_tac_toe

        if !rock_paper_scizzor.is_done():
            subaction roc_paper_scizzor



fun gen_printer_parser():
    let state : ComposedGame
    let any_action :  AnyComposedGameAction
    gen_python_methods(state, any_action)

fun score(RockPaperScizzorGame game, Int current_player) -> Float:
    if !game.is_done():
        return 0.0
    return float(game.tic_tac_toe.board.three_in_a_line_player(current_player)) + float(game.rock_paper_scizzor.winner.value == current_player)

fun get_current_player(RockPaperScizzorGame game) -> Int:
    if game.is_done():
        return -1
    let hand : HandType
    if can game.throw_hand_p1(hand):
        return 0
    if can game.throw_hand_p2(hand):
        return 1
    return game.tic_tac_toe.board.current_player()
```

In this example we run both game at the same time, executing a action of each until they are done. The `subaction statement` without the `*` allows to to exectly that, specify that one subaction must be executed instead of all.

# Conclusion

This was a introduction to the constructs you can find in `RL` and a explanation on how they help you at quickly applying machine learning to simulations. If you are interested in learning more, you can read as well our other documents:

