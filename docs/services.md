### Who are we and what do we do?

We are part of the HEAP lab at Politecnico di Milano, specializing in domain-specific languages and technologies aimed at solving problems within specific domains. This page refers to the **Rulebook** project, a set of tools we’ve created that allows users to quickly develop **digital twins** of action-based systems and interface them with machine learning tools.

You can contact us at **massimo.fioravanti@polimi.it**.

### What are action-based systems?

Action-based systems are systems where actors operate in the world by executing well-defined actions, one at a time. For example:
- Most **games** are action-based systems where the rules define precisely what kinds of actions players can take.
- **Robotic** arms are action-based systems where each joint and motor can be individually triggered as an action and combined with others to achieve a larger movement.
- **Economic** simulations aimed at predicting the return on investment of certain actions over time, such as buying and selling stocks, are action-based systems.
- **Logistical** distribution of goods aimed at minimizing transportation costs is an action-based system, where the action is the decision to allocate a particular good to a specific location at a specific time.

To simplify, all action-based systems can be thought of as games where there is a **state** (e.g., the position of a robotic arm, the state of an economic system), an **objective** (e.g., maximizing profit, moving the arm correctly), **rules** that specify what **actions** can be taken by **actors**, and how the **state** updates when actions are performed (e.g., the robotic arm can only move a certain distance, and if it does, it may knock down a brick).

### What is a digital twin?

A **digital twin** is a computer simulation that replicates the behavior of a physical object. For example, the computer version of a board game is a **digital twin** of the physical board game. A simulation of a car is a **digital twin** of the actual car. With precise **digital twins**, it is possible to observe a simulation of the real object in action and deduce its properties much faster and more cost-effectively.

### What is machine learning?

Machine learning is a field of science focused on creating computer systems that can learn, even in an unsupervised way. For example:
- Given a game like chess, machine learning techniques can learn to win games without human assistance, apart from being provided the rules.
- Given a robotic arm, it can learn to pick up a ball on a table.

In general, given the initial state, objective, and rules of an action-based system, machine learning techniques will learn to maximize that objective.

### What do we offer?

We offer consultancy services for creating and using **digital twins** for **action-based** systems. Using the earlier example of games, we can:
- Implement a **digital twin** of the game given its rules.
- Verify if the rules are correct or if they lead to invalid or impossible game states.
- Determine if the rules are such that the game may never terminate under certain conditions.
- Train a machine learning agent to play the game, depending on the characteristics of the game.
- Assess if the game is balanced, meaning all players have an equal chance of winning.
- Evaluate whether changes to the game rules improve or worsen the balance.
- Identify actions that are never taken by the agent, indicating they may be unexpectedly weak.
- Measure metrics about the game, such as the average number of turns a game lasts, how many cards are played, and more.
- Provide training to help you perform the above tasks on your own.

For example, you can read how we analyzed an off-the-shelf game [here](https://github.com/rl-language/rlc/blob/master/docs/space_hulk_level_design.md).

Furthermore, we offer the entire collection of tools that make up the **Rulebook** project for free, allowing anyone to replicate what we do.

### What do you offer that is better than similar services?

After spending several years solving technical problems that limited the speed at which **digital twins** could be produced, we are now able to provide these services much faster and with less human capital than others. While nothing prevents others from adopting the same techniques we use—which we freely provide for other programmers—the techniques are not yet widely adopted.

