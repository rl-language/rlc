# Rulebook Language

This document explains the language features of `Rulebook` and the features of the `rlc` compiler. The generated documentation of the `rlc` standard library can be found in this repository as well.

This is a technical document intended for advanced programmers. If you are a new user of Rulebook, refer instead to the [rlc repository page](https://github.com/rl-language/rlc) and to the [blackjack tutorial](https://github.com/rl-language/rlc?tab=readme-ov-file).

## RLC

`rlc` is the Rulebook compiler. It has a command-line interface similar to that of gcc and clang. Given a minimal `rl` file:

```rlc
# example.rl
import serialization.print

fun main() -> Int:
    print("hello world")
```

You can compile it from the command line with:

```bash
rlc example.rl -o compiled
```

And then test that it works with:

```bash
# Linux / macOS
./compiled
# Windows
./compiled.exe
```

The compilation pipeline works as follows:
* The file passed on the command line (example.rl in this case) is parsed, and so recursively are all the files named by `import` statements appearing in the input file.
* The program composed of all imported files is then compiled.
* The program is then linked against the rlcruntime library, yielding an executable file.

### Static and Shared Libraries

If you only wish to compile the program but not link it against the standard library, you can instead use the following command:

```bash
rlc example.rl -o example.a --compile # -o example.lib on Windows
```

This will produce a static library that will only depend on symbols exposed by the rlc runtime, as well as malloc and free. The compiled program will contain the symbol `rl_main`, but will not contain the symbol `main`, allowing you to redefine it if you wish. A pure `rl` program does not require invoking global functions before or after its execution. Therefore, you can just link a pure rl program in any program of your own and simply use its functions.

If you wish to create a dynamic library, you can do so with the command:

```bash
rlc example.rl -o example.so --shared # -o example.dll on Windows
```

This command will link the runtime library into the .so file, generating a dependency on whatever libc rlc finds in the system. The command will not define a `main` symbol, allowing you to use the shared object as you wish.

### C/CPP and Python Wrappers

At the moment, `rlc` can automatically generate C/CPP and Python wrappers, allowing you to use `rlc` programs from those languages.

* C headers can be generated with:
  ```bash
  rlc file.rl --header -o file.h
  ```
  This can then be included in a C/CPP program. You can take a look at the `rlc` [tests](https://github.com/rl-language/rlc/blob/master/tool/rlc/test/wrappers/action_with_frame_vars.rl) to see how to do this.

Similarly, you can run:
  ```bash
  rlc file.rl --python wrapper.py
  rlc file.rl -o lib.so --shared
  ```
  to produce a library you can import from Python. Once again, refer to the [examples](https://github.com/rl-language/rlc/blob/master/tool/rlc/test/wrappers/action_with_frame_vars.rl) to see how to achieve this.

### Optimization Level and Debug Symbols

Optimizations can be enabled with the flag `-O2`:
```bash
rlc file.rl -o file.exe # no optimizations
rlc file.rl -o file.exe -O2 # optimizations
```

Debug symbols can be emitted with `-g`.

## Rulebook Language

This section describes the language features of the `Rulebook` language.

### Rulebook Types

Rulebook types are composed of primitive types, array types, alternative types, function types, enum types, and class types. `rlc` types have the same ABI as `C` types, making them interoperable.

Primitive types are:
* **Int**, a signed 64-bit integer (`int64_t` in C)
* **Float**, a 64-bit floating point number (`double` in C)
* **Byte**, a signed 8-bit integer (`int8_t` in C)
  * For these numeric types, usual operations such as `+`, `-`, `*`, `/`, `%` are defined as built-in operations.
* **Bool**, an 8-bit type with the same behavior as C++ bool.
  * For Bool types, the following built-in operations are defined: `and`, `or`, `!`. `and` and `or` are short-circuiting.
* **StringLiteral**, a reference to a zero-terminated string literal.
* **OwningPtrType**, a special type used to implement the standard library. (ToDo: explain how it works)

### Enum Types

Enum types are declared with enum declarations, and they are converted to a class type that contains a single `Int` called `value`.

The syntax of enums is:
```rlc
enum Color:
  red
  blue

fun return_enum() -> Color:
  return Color::red

fun red_value() -> Int:
  return Color::red.value
```

Enums can declare functions in their body as well:
```rlc
enum Color:
  red
  blue
  fun name() -> String:
    if self == Color::red:
       return "red"s
    if self == Color::blue:
       return "blue"s
    return "unreachable"

fun red_name() -> String:
  return Color::red.name()
```

Finally, enums have a special syntax to write code such as the one in the last snippet faster. You can specify an enum member clause in each field of an enum.

```rlc
enum Color:
  red:
    String name = "red"s
  blue:
    String name = "blue"s

fun red_name() -> String:
  return Color::red.name()
```

The last two snippets generate the same code.

### Array Types

Array types are composed of a compile-time known size and an underlying type:
```rlc
let x: Int[10]
x[2] = 1
x[1] = x[2] + 2
```

Array accesses are checked by default on optimization level 0, and not checked on optimization level 2. The check can be enabled and disabled with `--emit-bound-checks=true/false`.

### Alternative Types

Alternative types are used to describe objects that can be of two different types. For example, an object that can be a `Float` or an `Int` can be declared as:
```rlc
let x: Float | Int
```

Declaration statements such as this one will initialize `x` to be an alternative that contains the first possible type of the list. In this case, the content will be `0.0`.

Objects of type alternative can be assigned from any right-hand operand that is either an alternative type with the same alternatives or any of the possible types of the alternative. For example:
```rlc
let x: Float | Int
x = 0     # valid
x = 1.0   # valid
x = true  # invalid

let y: Float | Int
y = 3     # valid
x = y     # valid
```

Alternative types can be upcasted with the use of type guards. For example, the following snippet of code is valid and will ensure that `y` will always be of type `Float`:
```rlc
let x: Float | Int
if x is Float:
  using T = type(x)
  let y: T
  y = 0.0
```

### Class Types

Class types are the most common type of user-defined type. A class includes both members and methods. For example, a tic-tac-toe board can be defined as follows:
```rlc
cls Board:
    BInt<0, 3>[3][3] slots
    Bool player_turn
```

Member variables whose names start with `_`, such as `_player_turn`, are not accessible from outside the file in which they have been declared.

The default value of a member when an object is constructed can be overridden by defining the `init` method:
```rlc
class IntegerThatStartsAt3:
  Int x
  fun init():
    self.x = 3

fun main() -> Int:
  let x: IntegerThatStartsAt3
  print(x)  # prints: {x: 3}
  return 0
```

If you override the `init` method, you will need to invoke the `init` method of all the member fields yourself.

The default destruction behavior can be overridden by defining a custom `drop` member function. This should be done only if you are doing some kind of low-level programming. If you override the destructor, you will need to call the destructor of the member fields yourself.

### Function Types

ToDo: explain how they work.

### Functions

RLC functions can be declared only in the global scope of a program. The simplest function that can be declared is a function with no arguments and no return values:
```rlc
fun NAME():
  return
```
The previous snippet declares a function called `NAME`, with no parameters and no return value, that does nothing.

Functions that return a value must declare their intention by specifying the return type before the `:` in their signature:
```rlc
fun return_5() -> Int:
  return 5
```
The previous snippet declares a function called `return_5` with no arguments that returns an integer.

Functions can have arguments by specifying them as a comma-separated list in the signature brackets:
```rlc
fun sum(Int first, Int second) -> Int:
  return first + second
```
Function arguments have a type and a name, and the type must be specified before the name.

All arguments of functions are passed by reference. Function return types may be marked with `ref`. If they are, the value returned by the function is not returned by copy, but by reference. More on this later, in the call expressions section.

#### Visibility

Functions whose names start with `_`, such as `_add`, are not visible from outside the file in which they have been declared.

#### Template Functions

ToDo.

### Action Functions

Action functions are the most important language feature of the `Rulebook` language. Action functions look like regular functions, except that:
* They are prefixed with `act`, instead of `fun`.
* The return type is not a return type, but instead is the `action function type`, a new type declared by the action function itself.
* `Action statements` must appear in the body of the action function.

For example, a simple action function you can write is:
```rlc
act play() -> Game:
    act set_x(Int value)
    frm x = value
    act print_x()
    print(x)
```

When the compiler encounters this action declaration, it rewrites it as a class that looks like:
```rlc
cls Game:
    Int x
    Int resume_index

    fun init():
        self.resume_index = 0
        self.x = 0

    fun set_x(Int value) {resume_index == 0}:
        self.x = value
        self.resume_index = 1

    fun print_x() {resume_index == 1}:
        print(self.x)
        resume_index = -1

    fun is_done() -> Bool:
        return self.resume_index == -1

fun play() -> Game:
    let game: Game
    return game
```

Why does it do that? Because then you can use the `play` function as follows:
```rlc
fun use_play():
    let game = play()
    game.set_x(4)
    game.print_x()
```
That is, when you were writing `play`, you expressed points of the program where the program was to be suspended, waiting for input from the invoker of `play`. In our particular example, it does not make much sense to do so. Here is a more realistic one:
```rlc
# Implements a vending machine that expects
# coins to be inserted by the user
act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        act insert_coin(Int coin_value) {coin_value == 1 or coin_value == 5 or coin_value == 10}
        target_cost = target_cost - coin_value

fun main() -> Int:
    let machine = vending_machine(16)
    print(can machine.insert_coin(3))  # false
    # machine.insert_coin(3)  # would crash
    print(can machine.insert_coin(1))  # true
    machine.insert_coin(1)
    machine.insert_coin(5)
    machine.insert_coin(10)
    print(machine.is_done())  # true
    return 0
```

The purpose of action functions is therefore to convert an imperative description of a procedure that requires inputs into a class that can be stopped and resumed at will.

Action functions cannot return objects; their return type is instead used to declare a new type.

### Frm and Ctx Qualifiers

Variable declarations, action statement arguments, and action function arguments can be optionally marked with the keyword `frm`.

For example:
```rlc
act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        act insert_coin(Int coin_value) {coin_value == 1 or coin_value == 5 or coin_value == 10}
        target_cost = target_cost - coin_value

fun main() -> Int:
    let machine = vending_machine(16)
    print(machine.target_cost)  # 16
    # print(machine.coin_value)  # does not compile
```

When a variable marked `frm` is promoted to be a local variable, it becomes a member of the class declared by the action function, in this case, the class `VendingMachine`. If a variable is used across actions, it must be marked as `frm`. This is necessary because to be used across actions, the variable must be saved somewhere between actions invocations by the users. If you forget to mark a variable as `frm`, the compiler will prompt you to do so if you use it between actions.

You can also mark variables not used across actions as `frm`. This is useful to expose data to the caller of the action function.

`ctx` variables are different. They specify that some state of the computation needed by the action function is stored outside of the action function itself. `ctx` variables must be passed as arguments of every function call to an object that has one.

For example:
```rlc
act vending_machine(ctx Int target_cost, frm Int target_cost2) -> VendingMachine:
    while target_cost != 0:
        act insert_coin(Int coin_value) {coin_value == 1 or coin_value == 5 or coin_value == 10}
        target_cost = target_cost - coin_value
        target_cost2 = target_cost

fun main() -> Int:
    let x = 16
    let y = 16
    let machine = vending_machine(x, y)
    machine.insert_coin(x, 1)
    print(x)  # 15
    print(y)  # 16
```

Since `target_cost2` is marked `frm`, it is copied within the variable `machine`, thus modifying `target_cost2` does not change `y`. `target_cost` is marked as `ctx`, and is not saved within the class `VendingMachine`. It must be passed every time a member function of `machine` (except `is_done`) is invoked.

Context variables are very useful when combined with `subaction` statements.

### Action Statements

Action statements are used to specify the points where user information is required. The minimal syntax is as follows:
```rlc
act action_function() -> ActionFunctionType:
    ...
    act wait_to_be_called()
```

Optionally, action statements can have arguments:
```rlc
act action_function() -> ActionFunctionType:
    act callme(Int x)

fun f():
    let state = action_function()
    state.callme(1)
```

Similarly, they can have preconditions:
```rlc
act action_function() -> ActionFunctionType:
    act callme(Int x) {x != 0}

fun f():
    let state = action_function()
    print(can state.callme(0))  # false
    print(can state.callme(1))  # true
```

### Actions Statements

Actions statements are used to express alternative actions, meaning actions that can be performed in any order. For example:
```rlc
act vending_machine(frm Int target_cost2) -> VendingMachine:
    while target_cost != 0:
        actions:
            act insert_5_coin()
                target_cost = target_cost - 5
            act insert_1_coin()
                target_cost = target_cost - 1
            act insert_10_coin()
                target_cost = target_cost - 10

fun main() -> Int:
    let machine = vending_machine(20)
    machine.insert_10_coin()
    machine.insert_10_coin()
    return 0
```

### Subaction Statements

Subaction statements allow for the composition of actions. Just like regular functions can call other functions, subaction statements specify that the computation of the current action function must be suspended until the user invokes an action of the specified subaction. For example:
```rlc
act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        actions:
            act insert_5_coin()
                target_cost = target_cost - 5
            act insert_1_coin()
                target_cost = target_cost - 1
            act insert_10_coin()
                target_cost = target_cost - 10

act vending_machine_times_two(Int first, Int second) -> VendingMachinePair:
    subaction* first_machine = vending_machine(first)
    subaction* second_machine = vending_machine(second)

fun main() -> Int:
    let machine = vending_machine_times_two(20, 2)
    machine.insert_10_coin()
    machine.insert_10_coin()
    machine.insert_1_coin()
    machine.insert_1_coin()
    return 0
```

The function `vending_machine_times_two` is lowered to the following piece of code:
```rlc
act vending_machine_times_two(Int first, Int second) -> VendingMachinePair:
    frm first_machine = vending_machine(first)
    while !first_machine.is_done():
        act insert_5_coin() {can first_machine.insert_5_coin()}
            first_machine.insert_5_coin()
        act insert_1_coin() {can first_machine.insert_1_coin()}
            first_machine.insert_1_coin()
        act insert_10_coin() {can first_machine.insert_1_coin()}
            first_machine.insert_10_coin()

    frm second_machine = vending_machine(second)
    while !second_machine.is_done():
        act insert_5_coin() {can second_machine.insert_5_coin()}
            second_machine.insert_5_coin()
        act insert_1_coin() {can second_machine.insert_1_coin()}
            second_machine.insert_1_coin()
        act insert_10_coin() {can second_machine.insert_1_coin()}
            second_machine.insert_10_coin()
```
That is, the computation of the outer action function will not resume until the inner action function named by the subaction statement is terminated.

If you instead wish to execute a single action statement, instead of executing them all the way to the end, you can use the subaction syntax without `*`. For example:
```rlc
act vending_machine(frm Int target_cost) -> VendingMachine:
    while target_cost != 0:
        actions:
            act insert_5_coin()
                target_cost = target_cost - 5
            act insert_1_coin()
                target_cost = target_cost - 1
            act insert_10_coin()
                target_cost = target_cost - 10

act vending_machine_times_two(frm Int first) -> VendingMachinePair:
    subaction*(first) first_machine = vending_machine(first)
    subaction*(first) second_machine = vending_machine(second)

fun main() -> Int:
    let state = vending_machine_times_two(10)
    state.insert_5_coin()
    state.insert_5_coin()
    if state.is_done():
        return 0
    return 1
```

This example shows how to share state between two subactions. The inner vending machines have their `target_cost` variable marked as `ctx`, and thus owned by the caller. The caller, `vending_machine_first_times_two`, uses two `subaction*` to execute both actions until the `target_cost` reaches 0. Since `target_cost` is zero, they will reach zero at the same time, so when the `first_machine` terminates, so immediately does `second_machine`, and thus `vending_machine_times_two`. The significant improvement is that with the syntax `subaction*(first)`, we are forwarding `first` to every invocation of methods of the subactions, removing the requirement for the function `main` to do so. `main` can simply invoke `state.insert_5_coin()`, unaware of how the state is organized within vending machines.

#### Alternative subaction statement
Sometimes you may require to execute one subaction among many depending on the state of the program. For example, maybe the user can select if they wish to play tic tac toe or chess, and then they play the selected game. In this case you can use a subaction statement on a alternative type, provided that all the types of the alternative describe a action function.

Here is a example
```rlc
act tic_tac_toe() -> TicTacToe:
    # omitted

act chess() -> Chess:
    # omitted

act play() -> Game:
    frm game : TicTacToe | Chess
    act play_chess(Bool do_it)
    if do_it:
       game = tic_tac_toe()
    else:
        game = chess
    subaction* game
```

#### Subaction statements on multiple values
Sometimes you may wish to express the concept that multiple sub actions are executing in parallel, and that the user can advance any of them in any order. For example, immagine a game where any player can perform its own actions indipendently from other players. In that situation you can use subaction statements with multiple values. Here is a example where player can play both tic tac toe and chess at the same time.

```rlc
act tic_tac_toe() -> TicTacToe:
    # omitted

act chess() -> Chess:
    # omitted

act play() -> Game:
    frm tic = tic_tac_toe()
    frm chess = chess
    subaction* tic, chess
```


### Context Variables in Subaction Statements

An important language feature built into subaction statements and context variables is the ability to share data between distinct subactions. Let's look at an example:

```rlc
act vending_machine(ctx Int target_cost) -> VendingMachine:
    while target_cost != 0:
        act insert_1_coin()
        target_cost = target_cost - 1

act vending_machine_times_two(Int first, Int second) -> VendingMachinePair:
    let first_machine = vending_machine(first)
    let second_machine = vending_machine(second)

    while !first_machine.is_done() and !second_machine.is_done():
        subaction
```

This example illustrates how to use context variables with subactions. Here is a more detailed version:

```rlc
act vending_machine(ctx Int target_cost) -> VendingMachine:
    while target_cost != 0:
        act insert_1_coin()
        target_cost = target_cost - 1

act vending_machine_times_two(Int first, Int second) -> VendingMachinePair:
    frm first_machine = vending_machine(first)
    frm second_machine = vending_machine(second)

    while !first_machine.is_done() and !second_machine.is_done():
        subaction first_machine
        subaction second_machine

fun main() -> Int:
    let state = vending_machine_times_two(10, 2)
    state.insert_5_coin()
    state.insert_1_coin()
    state.insert_5_coin()
    state.insert_1_coin()
    if state.is_done():
        return 0
    return 1
```

In this case, `vending_machine_times_two` is set up to handle two vending machines in parallel. Each machine runs independently but shares the control flow. This allows for flexible and efficient handling of multiple actions.

### Return Statements

Functions with a return type must include at least one return statement, and such a return statement must have an expression, as shown in the following snippet:

```rlc
fun sum(Int first, Int second) -> Int:
  return first + second
```

The expression in the return statement must have the same type as the function signature. When a return statement is encountered, the execution of the function immediately terminates, and statements that follow the return statement are not executed.

### If Statements

If statements can either include an else-branch or not. Here is the syntax for if statements without an else-branch:

```rlc
if bool_returning_expression():
  some_call1()
  some_call2()
some_call3()
```

The true-branch (e.g., `some_call1()` and `some_call2()`) is executed if and only if the condition expression evaluates to true (`bool_returning_expression()`).

If statements with an else-branch follow this syntax:

```rlc
if bool_returning_expression():
  some_call1()
  some_call2()
else:
  some_call3()
```

The else-branch is executed only if the true-branch is not.

### While Statements

While statements follow this syntax:

```rlc
while bool_returning_expression():
    call1()
    call2()
call3()
```

The body of the while loop keeps being executed until the condition expression (`bool_returning_expression` in the example) evaluates to false.

#### Break Statements

When a break statement is encountered, control flow jumps to the statement following the nearest while statement. For example, the following code prints `1` but not `0`:

```rlc
let x = 0
while x != 2:
    while x != 2:
        if x == 0:
            break
        else:
            print(x)
    x = x + 1
```

#### Continue Statements

When a continue statement is encountered, control flow jumps to the statement following the nearest while statement. For example, the following code prints `2` but not `1`:

```rlc
let x = 0
while x != 2:
    while x != 2:
        x = x + 1
        if x == 1:
            continue
        else:
            print(x)
```

### Using Statements

Type aliases can be introduced with the use of alias statements. For example:

```rlc
fun main() -> Int:
    using T = type(6)
    let asd : T
    asd = 10
    return asd - 10
```

Using statements can be used both at the function scope or at the global scope. Aliases introduced this way do not generate a new type; instead, they simply redirect all uses of the alias type to the aliased type.

Aliases offer the following syntax as well, which deduces the aliased type from the expression appearing inside the `type()` clause:

```rlc
using T = type(6)
```

### Declaration Statements

In RL, there are two flavors of declaration statements and three types of storage qualifiers. The simplest declaration statement is an assign declaration statement, in the form:

```rlc
let x = exp()
```

This deduces the type of `x` from the expression `exp()`, and removes a reference qualifier from the type if there is any, making a copy of `exp()` result if it was a reference.

Alternatively, declaration statements can be construction declaration statements, in the form:

```rlc
let x : Int
```

Construction declaration statements allocate a variable of the type specified after the colon and initialize them by invoking the member function `init` on them.

### Reference Declaration Statements

Instead of declaring a variable with the keyword `let`, `ref` can be used instead:

```rlc
ref x = ref_returning_exp()
```

In this case, `x` is not a new variable but a reference to the variable returned by `ref_returning_exp()`.

### Frame Declarations Statements

These special declaration statements are explained in the action function section.

### Declaration Statements Destruction

When declaration statements go out of scope, they are destroyed by invoking the member function `drop`.

### Call Expressions

To be added.
