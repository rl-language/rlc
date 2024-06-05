# Rulebook Introduction

RL is a reinforcement learning centring scripting language, aimed at delivering reinforcement learning techniques to user with no experience in machine learning, while allowing full control to the user that do know how to tune and modify machine learning components.

## Recap on reinforcement learning
Reinforcement learning is the field of science that study algorithms that learn make decisions to achieve the most optimal result.
Examples of reinforcement learning are,
* computer systems that learn to play chess by only knowing the rules of chess are reinforcement learning systems.
* car driving systems that learn to drive without crashing in a real or simulated environment.

Reinforcement learning life cycle is divided into two steps, learning and deployment. In the step of learning the reinforcement learning system interacts with a real or simulated environment so it can learn to perform better at whatever task it was designed for, for example a chess system may learn to play chess by playing against itself. When deployed the system stop learning and instead it actually performs the task it was inteded for in a real environment. For example, a chess system may play against humans.

When training reinforcement learning solutions, there are a sets of metrics that must be, ideally, maximized(or minimized).

* **Performance**: we want the learning system to become as proficient as possible about whatever task it was meant to learn. In theory, the final quality of learning of a given problem only depends on the learning algorithm and its configurations, which are bounded by humanity knowledge of theoretical machine learning. In practice it depends as on the qualify of the hardware, since faster hardware can allow to use techinques that performs worst on small training runs but perform better on longer training runs.
* **Learning speed**: we want the system to learn as fast as possible. Learning speed is both a property of the speed of hardware and of the known learning algorithms.
* **Costs**: regardless how what we want to produce, we wish to do so as cheaply as possible.

This 3 metrics are connected, decreasing the cost of hardware may allow to afford more hardware which may yield at the same time better learning speeds and performance results. Increasing the performance by two times may required may 10 times the costs.

## Rulebook and reinforcement learning

As we mentioned, the `Rulebook` langauge is meant to help you use reinforcement learning techniques. It does so by helping you write simulated environments in which machine learning agent can learn. Let consider again the previouly mentioned machine learning metrics and let us see which ones are helped by using `Rulebook`:

* **Learning speed**:
    * Simulation speed: to be able to learn, a machine learning algorithm must interact with a real or simulated environment to try actions and see what happens. The faster the a simulated environment executes, the faster the learner can learn. We will show how writing `RL` programs helps you write efficient simulated environements.
    * Development speed: a simulated environment in which a algorithm can learn is a software like any other. Writing, maintaining, and deploying it is a time consuming process and the fidelity of the simulation to reality is critical to obtain good quality results. We will show how using `RL` helps you create simulated environments faster, more easily than using an alternative language.
* **Costs**:
    * Development costs: As mentioned in the learning speed, mantaining a simulation of the environment in which to learn is a significant development costs. Simplyfing that aspect of development with the `RL` language helps reducing the costs debugging, updating and in general modifying the simulation.
    * Human costs: A team developing reinforcment learning solutions is costly to mantain, lots of skills are required, machine learning skills, development skills, skills related to the domain one is trying to learn, and so on. By simplifying the creation of digital environments in which the agents can learn, we allow smaller teams to create their products, instead of requiring multiple human resources with different skill sets.
    * Deployment costs: some `RL` agents are meant to operate in the real world, such as autonomous driving systems, and thus after deoployment the simulation in which the trained is no longer relevant. Some instead can operate in the same environment they have been trained on, for example a chess learning algorithm can learn and be deployed in the very same simulation of chess. In that case the simulated environment is no longer just a training utility, but something to be deployed to end user too. This may include including extra feature to the simulation, for example a way to render to screen the state of the simulation, a way for humans to interact with the simulation and so on. We will see how the `RL` language helps you to develop, mantain and deploy simulations beyond the scope of just learning.
    * Interoperability costs: sometimes it is necessary for machine learning systems to interact with some other prehexisting component. For example, maybe parts of the simulation from which the machine learning system is learning may be third party programs beyond the abilities of the programmer to modify. We will see how `RL` can interoperate with tools and libraries written in other languages.


Notice that we have not talked about final learned performaces. That is absolutelly intentional. The `RL` language does not provide new reinforcement learning methods, instead it concerns itself with the issue of writing simulations only. You can use any learning algorithm you wish when using a `RL` language simulation.
Of course, included in the `RL` langauge package we ship a off the shelf machine learning algorithms to be used by those that do not have the skill required to write a custom solution. We will se how a user with no machine learning knowledge can still use `RL` and obtain solutions for their own optimizations problems.

## About this document
As specfied by the previous sections, this documents aims at giving an overview of the `RL` language, providing a hands on approach were the reader may try the language on its own machine. By the end of this document you will learn:

* How you can write games and optimization problems in `RL`.
* How you can run, with zero configurations required, a neural network to find good strategies for your problem.
* How RL can interoperate with other languages, C, CPP and python, and how easy is to integrate in other codebases.
* What code, performance wise, you can expect to be generated from RL.

## WARNINGS:
This document is intended to be a proof of concept, the language has not yet been released under version 1.0, and the our intent is to collect feedback about the language.

**This document requires some understanding of programming. If you can program in python or a similar language, you should be able to follow the content presented here.**

As of the moment of writing 1/6/2024, games with multiple players do not work off the shelf due to issue in the library providing the builtin machine learning agent. If you are interested solving multi agents, contact us and we will guide you on how to solve the issues. Of course we plan to address this issue as soon as possible.

## Requirements

To be able to run the follwing examples you need:

* A linux x64 machine (we plan to support every major architecture and OS in the future, but at the moment we are limited by the system supported by libraries we depend upon).
* At least `python 3.8` installation with `pip`
* clang, which should be possible to install with `sudo apt install clang` or `sudo yum install clang`

Optionally, you can install

* xdot, used to visualize the game state machine
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

## Training and running

Now that you made sure your system works, download the `Sudoku` example from [../tool/rlc/test/examples/sudoku.rl](here), and save it in a file in the directory we created at the start of this example.

Read the example and make sure you understand it.
If you are interested in learning more about the mechanism described there you can try as well reading the files described here [../tool/rlc/test/tutorial/2.rl](here) and [../tool/rlc/test/tutorial/3.rl](here).


After you have copied it, you can run `rlc-learn` and see it learn
```
rlc-learn sudoku.rl -o network |& tee log.txt
```

If you have not installed torch and cuda, this invocation will fail. If it does fail, you can open the file log.txt and try see what you are missing to run it.

If it does learn, you can open the log and scroll down until you see a line that looks like the following.
```bash
tensorboard --logdir /tmp/ray/session_DATE/artifacts/DATE/tuner_run_DATE/driver_artifacts
```

Open another terminal and navigate to the example directory you have created, and source the environment again.

```bash
cd rl_example
source ./venv/bin/activate
```

Then run the line you have found in the log.txt file.
From your browser you can now visit the website.
```
127.0.0.1:6006
```

From there you should see a website that looks like this

![../imgs/tensorboard.png](tensorboard example)

In the search bar, write `episode_reward_mean`

You should see a graph that looks similar to the following

![../imgs/mean_reward.png](tensorboard example)

The x axis is the number of actions played, in the case of our game it means the number of cells that have been filled across multiple games by the learning agent.
The y axis rapresents the average score obtained by the learner. As you can see the more games it plays the better scores it gets, it is actually learning how to play!

Of course there is no guarantee that the machine will achieve a score of one, since we are generating random boards, and not all random boards are solvable.
Furthermore the size of the neural network has been defaulted to a resonable default, but there is no guarantee that the problem is solvable given the the default size.

Still, with very few commands, and a very simple `.rl` file we managed to have a resonably configured network up and learning.

Let it train for as long as you wish, we saw the training plateu after ~20 milions moves, at around 0.55 points, that is: the network learned to plan ahead so that in average it can play ~ten mores turns before being stuck. Notice that the games start with 20 randoms move already set, so a score of 0.55 means that the game managed to play up to ~70 moves total before getting stuck.

After you have interrupted the training, you can generate a game by running
```
rlc-play sudoku.rl network -o game.log
```

The command will create a file called `game.log` that contains a human readable game that you can inspect, or you can reproduce by running

```
rlc-action sudoku.rl game.log -pp
```

This command will run a action at the time and let you visualize the game by invoking pretty\_print after each action executed. You need to press any button to advance the game by one action.

## Building on top of it

So, until now we have seen how to write, train, run and visualize a game.Of course this is not the end of the road.
After you have trained a network, you probably wish to use the rules you have written it in a real environment. Let us see how to do so, by writing a python script that can interact with the `rl` sudoku implementation.

Craete a file called `example.py`, and write the following content
```python
import rlc
import random

# load the rl file
rl_module = rlc.load("sudoku.rl")
sudoku_game = rl_module.functions.play()

while rl_module.functions.get_current_player(sudoku_game) == -1:
  action = random.choice(rlc.enumerate_valid_actions(sudoku_game))
  rl_module.apply(game, action)

rl_module.functions.pretty_print(sudoku_game)
while sudoku_game.resume_index != -1:
    print("write x, y, number")
    x = input()
    y = input()
    number = input()
    user_action = rl_module.ActionMark()
    user_action.x.value = x
    user_action.y.value = y
    user_action.number.value = number
    if not rl_module.functions.can_apply(game, user_action):
        print ("provided action was invalid")
        rl_module.functions.print(action)
        continue

    rl_module.functions.apply(game, user_action)
    rl_module.functions.pretty_print(sudoku_game)
```

You can run this program with the following command, used a shell with the activated environment.
```
python example.py
```

As you can see, you are able to play sudoku, driven by a python script. Of course you could already do so with the `rlc-action` command provided by the `rl_language` package, but this example shows that `rl` program can be easily used from other languages such as python or cpp.
This allows you to reuse the same code `rl` code you have written to train the network in production too, building other tools of top of it!

You can as well load the network you have trained and use it to play games, but such setup is a little too complex to include it in this introductory document, and will be shown later.

## Conclusions
At the start of this document we described how machine learning works, and what `RL` wishes to do for it. In particular we said that would have learned

* How you can write games and optimization problems in `RL`.

As you saw `RL` lets you write games and simulations the way you conceputalize them, by declaring at each point which actions the user must take in that moment.

* How you can run, with zero configurations required, a neural network to find good strategies for your problem.

As you was, it takes a single command to have the machine learning components up and learning. Of course it is not guaranteed that the default setup will yield optimal results, but it is a starting point for those that can tune the machine learning configurations to their needs, and very to use machine learning for those that cannot.

* How RL can interoperate with other languages, C, CPP and python, and how easy is to integrate in other codebases.

We saw how other programs can interact with the simulation by importing a `RL` file and invoking functions and actions declared in that file, without the need of reliquishing the main loop of the program to the `RL` part of it. In our example we show how to do so in python, but it could have been any other programming language, or environment, such as graphic engines and so on.

* What code, performance wise, you can expect to be generated from RL.

RL is a compiled language, not a interpreted language, yielding performances comparable with other compiled languages such as C.
