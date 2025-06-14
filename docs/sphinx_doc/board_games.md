# Board games

This page describes how one can write and test board games. Using **Rulebook**, you can reduce the code complexity of digital board game implementations by at least an order of magnitude.
This page assumes that the reader is familiar with the typical characteristics of board games, and enough knowledge of programming to be able to implement a game of the complexity of **Risk**.

---

## The history of board game programming
Board games have long been an area of both interest and disinterest for programmers.
* **Abstract board games** have been deeply analyzed for the purpose of developing traditional AI systems[[1]](https://en.wikipedia.org/wiki/Deep_Blue_(chess_computer)), [[2]](https://en.wikipedia.org/wiki/AlphaGo). Abstract board games typically include a low number of distinct game rules that yield complex emergent dynamics.
* **Commercial board games** instead have been of little interest for the field of artificial intelligence, as well as of low interest for the field of video game programming. **Commercial board games** often have large amounts of rules written on game components, such as **Magic: The Gathering** cards, that offer players complex sequences of actions, while the total amount of information a game state presents is usually low because the user must keep track of it mentally. In contrast, video games often provide large amounts of information only when needed, while single game sequences are simpler. For example, in many games you can press any button of any interface in any order. While there exist various digital implementations of board games—for example [Board Game Arena](https://en.boardgamearena.com)—in practice the broader video game programming ecosystem is not tuned for the development of digital board games.


---

## Board games and their implementations

When a board game is played its rules are **interpreted** by the human players as if the players were computers themselves. Whenever the game requires some input from the players, the players decide how the game should continue, and then the players resume **interpreting** the rules.

When board games are designed, there is no concern in how game rules impact the complexity of writing code that implements the same rules. This leads to very complex game rules that are hard to write. For this reason, board games are the supreme [complex interactive programs](./the_inversion_of_control_problem.md#complex-interactive-subsystems).

The following is a control flow diagram of the game **Hanabi**, showing its only sequence.
```{graphviz}
digraph Hanabi_Control_Flow {
    rankdir=TB;                             // vertical (Top → Bottom)
    fontname="Helvetica";

    /* ---------- node & edge defaults ---------- */
    node [shape=diamond, style=rounded, fontname="Helvetica"];   // decisions
    edge [fontname="Helvetica"];

    /* ---------- main loop ---------- */
    Start  [shape=circle, label="Start\nturn"];
    End    [shape=circle, label="End\nturn"];

    Start -> Action;

    Action [label="Choose action:\n• Give Hint\n• Play Card\n• Discard Card"];

    /* ---- Give-Hint branch ---- */
    Action       -> GiveHint     [label="Give Hint"];
    GiveHint     [label="Select teammate\nto hint"];
    GiveHint     -> HintType;
    HintType     [label="Choose hint content:\n• Color\n• Number"];
    HintType     -> End          [label="Reveal hint"];

    /* ---- Play-Card branch ---- */
    Action       -> PlayCard     [label="Play Card"];
    PlayCard     [label="Select card\nto play"];
    PlayCard     -> End          [label="Attempt play"];

    /* ---- Discard-Card branch ---- */
    Action       -> DiscardCard  [label="Discard Card"];
    DiscardCard  [label="Select card\nto discard"];
    DiscardCard  -> End          [label="Discard + draw"];

    /* ---- Loop to next player ---- */
    End -> Start  [style=dashed, label="Next player's turn"];
}
```

As of the moment of writing general board game programming often follows the same pattern (not necessarily executed in the presented order).

* **The data structures of the game are implemented**, such as decks of cards, the board of the game, the resources available to players...
* **The game sequences are implemented**, which manipulate the content of the game data structures. A game sequence may be a rule that tells you to roll a dice, and if the roll results in a 6, you can select a player and take a point from them.
* **The UI is assembled**, deciding which game components are shown where, and deciding how game sequences are displayed.
* **The UI is fitted with art**, where the art is usually at least partially available due to the existence of the physical board game implementation.


Here are a couple of examples that show the same pattern.
* The implementation of [coup on Board Game Arena](https://github.com/quietmint/bga-coupcitystate), which has the data structures written in **SQL**, the game sequences written in **PHP**, the UI written in **JavaScript**, and the images taken from the original game.
* The implementation of [Hanabi by Google DeepMind](https://github.com/google-deepmind/hanabi-learning-environment) for the purpose of testing machine learning systems, which implements the data structures of the game as **CPP** classes, the game sequences as **CPP** state machines, and then specifies the UI as a mere printing function.

---

## The pressure on Game Sequences.

