Language Philosophy

This a intentionally rambly document, if you with to read a more structured one first, read [this](./rationale.md).

## The issue we are trying to solve

RL was born from pain. The pain of simulation rules brittleness, hardness of refactoring, and poor reusability that plagued most of my programming life.
Why is it so hard to take the source code of a fully working car flow simulation and turn it into something compatible with a machine learning framework? Why is it so complex to take a level from a completed video game and run a fuzzer on it? Why can't I automatically generate a command line interface and a network protocol from the rules of some economical simulation Pytorch learned to maximize?

The knowledge required to do these transformations is all in the program. The program knows how to run successfully. It must if it wishes to interact correctly with the user. Yet, that information is not available to the programmer.

The business logic control flow of the program is held hostage by a terrorist with a terrible weapon, the main loop of the process.

Machine learning frameworks, game engines, and web servers are components so critical to performance that the business logic built on top of them ends up dismembered and disseminated around the code.

That is rarely an issue. In the case of a web server, the server content often is a resource stitched together from multiple parts. If you need to train a network to recognize cats, you can preprocess the cat images from a reasonable format into whatever bizarre format your network needs.

It only turns into an issue when the business logic code is a complex graph.
Is your web page a complex legal form that requires multiple conditional steps? Well, the main loop belongs to the browser. You will turn that procedure into a state machine where clicking on buttons moves you from one state to the next, by redirecting to another page.
Does your game have scripts that coordinate actors shown on screen?
The actors belong, as the main loop does, to the rendering pipeline. Your scripts must explicate their state machine to check every frame if the state needs to advance.

It is unavoidable. The main loop belongs to the engine. The engine needs it most. Any design that diverges from this idea will leave performances on the table by constructing some weird architecture that puts extra burdens on the engine.

So, where do we go from here? Are we bound to eternally rewrite complex procedures as state machines with the method "update" invoked by the engine?
Well, let us see, who has solved this issue? Compiler people did. Every machine has a final main loop, the main loop of the CPU. Instructions are fetched, decoded, and executed. Instructions are not the main loop owner, the CPU is. All the instructions can do is optionally specify what is to be executed next, through jump instructions. If you write assembly code you have to do this yourself. You must manually specify how the control flow of the program evolves by only knowing the current operation and the register states. You need to explicate the state machine of the whole program.

The compiler hides the requirement of manually specifying the successor of each instruction. It is implied in the relative position of statements in your program, and with that information it will take care of jumps.

But then that is the fundamental nature of our issue. When we write scripts for a piece of code that has no ownership of the main loop, we are still using a compiler or an interpreter that is written expecting to target the main loop of the machine, the CPU. We cannot write an endless for loop without taking over the main loop.

We need a business logic compiler that can turn a procedure into the same state machine as a regular compiler, except it does not assume to know what the main loop is, nor does it try to take it over.

Well, that is a nice-sounding, poorly-defined idea, isn't it? How can we possibly do that? Well, what do people do? They take a program that exists in their head as a procedure, and then they cut it apart at some particular spots where the program cannot continue before some input from some other part of the system is provided.

That is it, that is all the compiler needs to do differently than usual. And that is what RL does. You can specify in your programs particular points where the computation must stop, and control must be given back to the caller. Then the caller decides when and if to restart you. Your program will be written as a procedure, compiled down to a state machine, and the engine will keep the ownership of the main loop.

We already have similar concepts. Coroutines do the same. They express a function that can be stopped and resumed. Indeed this is exactly what we are trying to do, except we are trying to bring this concept to its logical conclusion. The coroutine body must be allowed to collaborate with the type-checking mechanism of the language and introduce more types and functions than just the start, resume, and is_done functions.

## Design principles

Now that we know that is the issue we are trying to solve, we are better off deciding what design principles to stick with, when designing the language.

##### Metadesign principle

Since design principles are themselves designed, we should have a general rule for them too. The general rule is "every design principle should help solve the practical issue of writing programs abstracted out from the main loop owner, while still yielding a useful tool".

##### Interoperability
The problem we are trying to solve is that reusing code written for a given engine in another context is hard. This implies that interoperability is the whole point of what we are trying to do. RL must be a simple-to-integrate language, ideally as simple as integrating C.

##### Independence from the underlying machine
The abstract machine of the language should not talk in terms of the underlying machine, because that will make porting programs that rely on a specific machine's behavior hard, defeating the whole point of the language.

##### Speed
We cannot know who will use this language, we cannot make an if statement run slower than it runs in C, if we do, people will sometimes need to go back to C to achieve the performance required.

##### Memory independence
 We should not make assumptions on anything related to memory other than the existence of malloc. The code emitted should not introduce more malloc invocations than it would be done by writing the code in C.

##### Composability
Composing previously existing things is how progress in functionality is achieved. We should be allowed to express any such kind of composition. If a user feels that something cannot be expressed, it will write the same code in C instead.

##### All things decay
 All compilers bloat and become slow, and all languages swell and turn too complex. The initial language and compiler should be as small as possible, but not less.
