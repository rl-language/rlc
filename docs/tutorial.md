# Rulebook Introduction

RL is a reinforcement learning centring scripting language, aimed at delivering reinforcement learning techniques to user with no experience in machine learning, while allowing full control to the user that do know how to tune and modify machine learning components.

## Recap on reinforcement learning
Reinforcement learning is the field of science that study algorithms that learn make decisions to achieve the most optimal result.
Examples of reinforcement learning are,
* computer systems that learn to play chess by only knowing the rules of chess are reinforcement learning systems.
* car driving systems that learn to drive without crashing in a real or simulated environment.

Reinforcement learning life cycle is divided into two steps, learning and deployment. In the step of learning the reinforcement learning system interacts with a real or simulated environment so it can learn to perform better at whatever task it was designed for, for example a chess system may learn to play chess by playing against itself. When deployed the system stop learning and instead it actually performs the task it was inteded for in a real environment.

When training reinforcement learning solutions, there are a sets of metrics that must be, ideally, maximized(or minimized).

* **Performance**: we want the learning system to become as proficient as possible about whatever task it was meant to learn. In theory, the final quality of learning of a given problem only depends on the learning algorithm and its configurations, which are bounded by humanity knowledge of theoretical machine learning. In practice it depends as on the qualify of the hardware, since faster hardware can allow to use techinques that performs worst on small training runs but perform better on longer training runs.
* **Learning speed**: we want the system to learn as fast as possible. Learning speed is both a property of the speed of hardware and of the known learning algorithms.
* **Costs**: regardless how what we want to produce, we wish to do so as cheaply as possible.

This 3 metrics are connected, decreasing the cost of hardware may allow to afford more hardware which may yield at the same time better learning speeds and performance results. Increasing the performance by two times may required may 10 times the costs.

## Rulebook and reinforcement learning

As we mentioned, the `Rulebook` langauge is meant to help you use reinforcement learning techniques. It does so by helping you write simulated environments in which machine learning agent can learn. Let consider again the previouly mentioned machine learning metrics and let us see which ones are helped by using `Rulebook`:

* **Learning speed**:
    * Simulation speed: to be able to learn, a machine learning algorithm must interact with a real or simulated environment to try actions and see what happens. The faster the a simulated environment executes, the faster the learner can learn. We will show how writing `RL` programs helps you write efficient simulated environements.
    * Development speed: a simulated environment in which a algorithm can learn is a software like any other, writing, maintaining, and deploying it is a time consuming process and the fidelity of the simulation to reality is critical to obtain good quality results. We will show how using `RL` helps you create simulated environments faster, more easily than using an alternative language.
* **Costs**:
    * Human costs: A team developing reinforcment learning solutions is costly to mantain, lots of skills are required, machine learning skills, development skills, skills related to the domain one is trying to learn, and so on. By simplifying the creation
    * Development costs: As mentioned in the learning speed, mantaining a simulation of the environment in which to learn is a significant development costs. Simplyfing that aspect of development with the `RL` language helps reducing the costs debugging, updating and in general modifying the simulation.
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

Now that you made sure your system works, download the `Sudoku` example from [../tool/rlc/test/examples/sudoku.rl](here), and save it in a file in the directory we created at the start of this example.

Read the example and make sure you understand it.
