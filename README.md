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

Zero mallocs unless explicitly requested by the user.

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

[Paper for Reinforcement Learning users](https://arxiv.org/abs/2504.19625)

### Contacts

[Discord](https://discord.gg/saSEj9PAt3)
[Twitter](https://twitter.com/RulebookL3873)
[Youtube](https://www.youtube.com/watch?v=tMnBo3TGIbU)
Or mail us at massimo.fioravanti@polimi.it



![RLC Logo](./imgs/RLC_logo.png)

### License

We wish for `RLC` to be usable by all as a compiler, for both commercial and non-commercial purposes, so it is released under apache license.





