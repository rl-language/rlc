# RLC

This document describes the architecture of `RLC`, the rulebook compiler, and of the other tools of the RLC suite. The intent is to provide a high level description of how the various parts of Rulebook fit into a product built on top of it, as well as a description of RLC command line facilities. More exhaustive descriptions of all tools can be found in the [reference](./language-reference.md).


RLC is a [LLVM](https://llvm.org) based compiler, it ingest one or more rulebook files and various possible outputs depending on the user request.

```{graphviz}

digraph RLC_Compiler {
    /*––Layout––*/
    rankdir = LR;
    nodesep = 0.6;
    ranksep = 1;

    /*––Global node / edge styling––*/
    node [shape=box, style=filled, color="#dddddd",
          fontname="Helvetica", fontsize=10];
    edge [arrowsize=0.7];

    /*––Input & compiler––*/
    rl_file [label="file.rl",
             shape=note, style="filled,rounded",
             color="#8ecae6", fontcolor="#0a3045"];
    rlc     [label="rlc",
             shape=box3d, color="#219ebc", fontcolor="white"];

    /*––Outputs, all kept on the same vertical line––*/
    subgraph cluster_outputs { rank=same;
        executable [label="Executable"];
        shared     [label="Shared Library"];
        static     [label="Static Library"];
        header     [label="header.h"];
        godot      [label="Godot-CPP Module"];
        csharp     [label="C# Wrapper"];
        python     [label="Python Wrapper"];
    }

    /*––Edges––*/
    rl_file -> rlc;

    rlc -> executable;
    rlc -> shared  ;
    rlc -> static ;
    rlc -> header;
    rlc -> godot;
    rlc -> csharp  ;
    rlc -> python;
}
```

RLC produces:
* **native executable**, pretty much the same as the output of a c/cpp file compiled with clang.
* native libraries to reuse in other languages.
* a wrapper to use the library in that language.

## Command line options

The `rlc` executable supports a large number of command line flags.  Most of
them mirror the options of typical C/C++ compilers.  The options can be grouped
in a few categories: **introspection**, **output generation**, **compilation** and
**miscellaneous** features.  Below is a short description of every available
flag.

### Introspection options

* `--token` – lex the input file and print the token stream.
* `--dot` – dump the action graph in GraphViz format.
* `--graph` – dump a machine parsable description of the action graph.
* `--graph-filter <regex>` – print only actions whose name matches the regular
  expression.
* `--graph-only-actions` – omit non action functions from the dumped graphs.
* `--graph-inline` – inline calls inside the graph visualization.
* `--unchecked` – print the AST before type checking and exit.
* `--type-checked` – print the AST after type checking but before template
  expansion and exit.
* `--before-template` – dump the IR just before templates are instantiated.
* `--after-implicit` – dump the IR after implicit instantiation has completed.
* `--flattened` – dump the IR after control flow has been flattened.
* `--rlc` – print the final RLC IR and exit.
* `--ir` – dump the generated LLVM IR.
* `--mlir` – dump the MLIR representation.
* `--print-ir-on-failure` – if compilation fails, print the IR that caused the
  failure.
* `--timing` – print the time taken by each compiler pass.
* `--hide-position` – do not print file and line information in IR dumps.
* `--hide-dl` – do not print the module data layout in IR dumps.
* `--print-included-files` – instead of compiling, print the contents of all
  files included during parsing.  Use `--hide-standard-lib-files` to omit files
  coming from the standard library.

### Output generation

* `--header` – emit a C header describing the compiled module and exit.
* `--c-sharp` – emit a C# wrapper and exit.
* `--python` – emit a Python wrapper and exit.
* `--godot` – emit a wrapper for Godot and exit.
* `--compile` – generate an object file without linking.
* `--shared` – produce a shared library instead of an executable.
* `-o <file>` – path of the output file to generate.
* `--clang <path>` – path to the `clang` binary used for linking.
* `--rpath <path>` – add an rpath entry to the produced binary (may be repeated).

### Compilation flags

* `-O2` – enable optimizations and disable runtime checks.
* `-g` – generate debug information.
* `--emit-precondition-checks` – insert checks for function preconditions
  (enabled by default, disabled by `-O2`).
* `--emit-bound-checks` – insert array bounds checks (enabled by default,
  disabled by `-O2`).
* `--sanitize` – enable runtime sanitizer instrumentation.
* `--fuzzer` – build the program as a fuzzer; implies `--sanitize`.
* `--fuzzer-lib <path>` – path to the library implementing the fuzzer runtime.
* `--pylib` – link against the Python interpreter.  When enabled assertions use
  the symbol `rl_py_abort`.
* `--pyrlc-lib <path>` – path to the Python wrapper support library; enables
  `--pylib` automatically.
* `--runtime-lib <path>` – path to the Rulebook runtime library.  If omitted the
  compiler uses the runtime distributed with `rlc`.
* `--target <triple>` – override the default target triple.
* `--abort-symbol <name>` – specify the symbol to call when an assertion fails.
* `--MD` – generate a makefile style dependency file alongside the output.
* `--expect-fail` – return exit code 0 if compilation fails and 1 on success
  (useful in tests).
* `--verbose`/`-v` – print verbose information while invoking external tools.

### Miscellaneous

* `-i <dir>` – additional include directories searched when resolving imports.
* `--pylib` and `--pyrlc-lib` – support linking against Python as described
  above.
* Extra object files or additional `.rl` sources can be passed on the command
  line after the main input file.  Directories specified with `-i` are searched
  for imported files.

## rlc-lsp and autocomplete

rlc-lsp is a [language server](https://en.wikipedia.org/wiki/Language_Server_Protocol) for the Rulebook language. It allows users to get autocomplete in their ide.

Notice that rlc-lsp must be in PATH, so if you are using the PIP package of rulebook, you must start your editor from the shell that has already enabled the virtual environment that installed rl\_language or rl\_language\_core.

### VSCODE
`vscode` has a plugin available in the plugin store called `rl-lsp` and `rl-language` that enables autocomplete and syntax highlighting.

### VIM with YouCompleteMe
On `vim`, if you use `YouCompleteMe` you can get access to automplete by adding the following to your .vimrc file.

```
au BufRead,BufNewFile *.rl set filetype=rl

let g:ycm_language_server = [ {
            \    'name': 'rulebook',
            \     'cmdline': [ 'rlc-lsp', '--stdio' ],
            \     'filetypes': [ 'rl' ]
            \   }]

```

## rlc-test

rlc-test runs all functions with no arguments found in the input file that return a bool called test_\* .

```rlc
fun test_return_success() -> Bool:
    return true
```

```bash
rlc-test file.rl
```

This program is only available in a pip installation.

## rlc-random

rlc-random runs random actions on a program with a finite amount of actions, and does until it reaches the end of the program. It prints the selected actions.

This tool is useful as a command line utility to quickly test a interactive program, or to produce a trace useful to some other program.

To be usable with rlc-random, a program just requires that to have a entry point with the following signature `act play() -> Game`, and that all `action statements` that appear in `play` are enumerable.

This program is only available in a pip installation.

## rlc-learn

rlc-learn uses an off-the-shelf implementation of PPO to maximize some metric in a given Rulebook program. This tool is intended to be used to perform sanity checks by those that wish to roll out their own machine learning algorithm, or by those that do not have knowledge of reinforcement learning and wish to use an acceptable off-the-shelf implementation. A basic tutorial is shown [here](./tutorial.md).

The command accepts a number of options controlling how the program is
compiled and how the training loop behaves.  All options from
`make_rlc_argparse` are available together with a set of flags specific to
reinforcement learning.  The following list explains each option in depth:

* `--include`/`-i <dir>` – add `<dir>` to the search path for additional
  `.rl` files when compiling the environment.  Use this when your Rulebook
  program imports modules stored in other directories.  The flag may appear
  multiple times and each directory is forwarded to the compiler.
* `--stdlib <dir>` – path to an alternative Rulebook standard library.
  By default the version bundled with the installed package is used.  Specify
  this option if you maintain a custom copy of the standard library or if you
  are experimenting with modifications.
* `--pyrlc <path>` – location of the Python wrapper runtime library.
  The wrapper produced by `rlc --python` depends on this support library.
  Normally it is found automatically but installations in unusual locations may
  require setting this path explicitly.
* `--rlc`/`-c <path>` – path to the `rlc` compiler executable.  If the compiler
  is not available in your `PATH`, for instance when using a development build,
  point this flag to the desired executable so `rlc-learn` can invoke it.
* `--runtime`/`-rt <path>` – path to a custom Rulebook runtime library to link
  against.  Use this to train with an instrumented runtime or an experimental
  build instead of the one shipped with the package.
* `--extra-rlc-args <args>` – additional command line arguments forwarded
  verbatim to `rlc`.  Any string placed here is appended to the compilation
  command.  Typical uses include enabling optimizations (`-O2`) or runtime
  sanitizers without modifying the learning script.
* `--output`/`-o <file>` – file where the trained network is written.  After
  training completes the model parameters are saved in PyTorch format at this
  location.  The resulting file can be passed to `rlc-play` or `rlc-probs` and
  defaults to `network` when omitted.
* `--no-tensorboard` – do not launch the TensorBoard server.  By default
  `rlc-learn` starts TensorBoard on port `6006` and logs reward and loss curves
  for real‑time monitoring.  Disable it in headless or resource constrained
  setups.
* `--total-steps <n>` – total number of environment interactions collected
  before training ends.  Increasing this budget allows the algorithm to gather
  more experience and usually yields a stronger policy, albeit at the cost of
  longer training time.
* `--lr <value>` – learning rate used by the optimizer.  This controls the
  step size of gradient updates and heavily influences training stability.
  Values are typically between `1e-3` and `1e-5` depending on the environment.
* `--entropy-coeff <value>` – weight of the entropy bonus encouraging
  exploration.  A larger coefficient drives the agent to sample actions more
  uniformly, helping exploration but delaying convergence if set too high.
* `--clip-param <value>` – PPO clipping threshold.  This bounds the change in
  action probabilities between successive updates.  Small values lead to very
  conservative updates while larger ones allow faster but less stable learning.
* `--league-play` – periodically save networks and pit the agent against older
  versions to prevent forgetting.  This self-play mechanism exposes the model to
  past behaviors so new strategies do not overwrite previously learned skills.
* `--load <file>` – start training from the checkpoint in `<file>` rather than
  from scratch.  Use this option to continue a previous run or fine tune an
  existing model on a new task.
* `--steps-per-env <n>` – number of steps collected in each environment before
  a gradient update (default `100`).  Larger values increase the batch size used
  for optimization and can improve stability, but they also raise memory
  requirements.
* `--hypersearch` – run a predefined hyperparameter search over learning rate,
  entropy coefficient, clip parameter and the number of steps per environment.
  Results for each configuration are stored in separate directories to simplify
  comparison.
* `--model-save-frequency <n>` – write a checkpoint every `<n>` iterations
  (default `20`).  Frequent checkpoints let you inspect intermediate policies
  or resume training after unexpected interruptions.
* `--envs <n>` – number of parallel environments used to gather experience.
  Each environment runs in its own process and contributes samples
  simultaneously.  Higher values speed up data collection but demand more CPU
  time and memory.

This program is only available in a pip installation.

## rlc-play

Given a Rulebook program file.rl, and the network obtained from `rlc-learn file.rl`, rlc-play generates a playout of that environment from start to finish according to the probabilities assigned by the network. A basic tutorial is shown [here](./tutorial.md).

This program is only available in a pip installation.


## rlc-probs

rlc-probs uses the network generated by rlc-learn to display on screen the actions available to the network in a given state of the input interactive program, and their respective probabilities. This tool is intended to be use to inspect the decision making skills of a trained network, beside gaining more insight than observing a single decision at the time.

A basic tutorial is shown [here](./tutorial.md).

This program is only available in a pip installation.



## rlc-action

Applies a execution trace to a rulebook program, and then prints the final result. This tool is useful to check if a trace is valid, if the traced program is correct.

The command accepts the standard compilation flags exposed by
`make_rlc_argparse` together with a few options controlling how the trace is
executed.  Below is a description of every available flag:

* `source_file` – path to the `.rl` source file (or a Python wrapper produced
  with `rlc --python`) to run.
* `action_file` – text file containing one action per line.  Use `-` to read
  from standard input.
* `--load`/`-l <file>` – load a previously saved binary state before applying
  the actions.
* `--output`/`-o <file>` – write the final state to `<file>` in binary format
  instead of printing it on screen.
* `--ignore-invalid`/`-ii` – skip actions that fail to parse or cannot be
  applied rather than stopping with an error.
* `--print-all`/`-all` – echo each line from the trace before executing it.
* `--pretty-print`/`-pp` – pause after each action and pretty print the current
  state, waiting for user input to continue.
* `--show-actions`/`-a` – list all available actions defined in the program and
  exit.
* `--include`/`-i <dir>` – additional include directories used when compiling
  the Rulebook program.
* `--stdlib <dir>` – path to a custom standard library.
* `--pyrlc <path>` – location of the Python wrapper runtime library.
* `--rlc`/`-c <path>` – explicit path to the `rlc` compiler.
* `--runtime`/`-rt <path>` – alternative runtime library to link against.
* `--extra-rlc-args <args>` – extra command line arguments forwarded verbatim
  to `rlc` during compilation.

This program is only available in a pip installation.


## Tools for compiler people

This section of the document talks about the internal components of RLC, and is meant for compiler developers, not users of the language. If you are confused about what this means and you are wondering if this section is for your, it is not, although you can still read it if you are interested in knowing what goes on under rlc hood.

## RLC internals

RLC is a [LLVM](https://llvm.org) and [MLIR](https://mlir.llvm.org) based compiler. It has a custom recursive descent parser, and a custom AST built on top of MLIR, called the RLC Dialect.

```{graphviz}

digraph RLC_Pipeline_V {
    rankdir = TB;           /* vertical (top-to-bottom) layout          */
    nodesep = 0.6;
    ranksep = 1;
    dpi     = 100;          /* higher resolution                        */

    node [shape=box, style=filled, color="#dddddd",
          fontname="Helvetica", fontsize=11];
    edge [arrowsize=0.7];

    /* main pipeline */
    lexer                 [label="Lexer"];
    parser                [label="Parser"];
    unchecked_ast         [label="Unchecked AST"];
    typechecked_ast       [label="Typechecked AST"];
    implicit_instantiation[label="Implicit Instantiation"];
    flattened             [label="Flattened"];
    action_rewriting      [label="Action Rewriting"];
    llvm_ir               [label="LLVM IR"];
    file_a                [label="file.a"];
    linker                [label="Linker"];
    lib_so                [label="lib.so"];

    /* auxiliary */
    libruntime [label="libruntime", shape=note,
                style="filled,rounded", color="#8ecae6",
                fontcolor="#0a3045"];
    wrappers   [label="Wrappers"];

    /* sequential flow */
    lexer  -> parser
    parser -> unchecked_ast
    unchecked_ast -> typechecked_ast
    typechecked_ast -> implicit_instantiation
    implicit_instantiation -> flattened
    flattened -> action_rewriting
    action_rewriting -> llvm_ir
    llvm_ir -> file_a
    file_a  -> linker
    linker  -> lib_so

    /* extra edges */
    libruntime -> linker ;
    implicit_instantiation -> wrappers;
}
```

### Lexer
The lexer of RLC takes the input file and turns them into token to be consumed by the parser. There is nothing fancy here except that since the language has semantical whitespaces, the lexer must keep track of the indentation.

You can see the token stream with `rlc --token file.rl`

### Parser
The parser is a recursive descent handwritten parser, the parser takes the tokens, and creates on the fly the unchecked AST.


### Unchecked AST
Before typechecking the AST of the language is unchecked, it means that almost every non trivial operation is marked as having unknown type.

You can see the token stream with `rlc --unchecked file.rl`

### Typechecked AST

The typechecked IR knows all types of all expressions, except for templates, which have been typechecked, but are not expanded yet.

Ideally, all static errors in the input program should be discovered during typechecking. In practice this is true for every error except errors relating to `drop`, `init` and `assign` when they are used inside templates.

You can see the token stream with `rlc --type-checked file.rl`


### Implicit instantiation

During the implicit instantiation step templates end up being generated, and implicit init, drop and assign functions emitted.

After the implicit step, no object of the module can be a template, and all non external declared entities must have been discovered.

You can see the token stream with `rlc --after-implicit file.rl`

Wrappers for other languages are emitted after this step, when all concrete functions are accounted for.

### Flattened

During the flattening step all control flow constructs are removed and replaced with jumps.

You can see the token stream with `rlc --flattened file.rl`

### Action rewriting

During action rewriting all Action Functions `act $name() -> $Type:` end up being replaced with the functions they declare. This includes:
* `fun $name() -> $Type:`
* `fun can_$name() -> Bool:` if the action function `$name` has a precondition.
* `fun $act($Type, args...)` for each action statement called `$act` in `$name`
* `fun can_$act($Type, args...) -> Bool` for each action statement called `$act` in `$name`
* `fun is_done($Type) -> Bool`

After action rewrting the module contains only types and functions.

### Lowering to LLVM IR

The module is translated to LLVM IR to be optimized and compiled.

You can see the token stream with `rlc --ir file.rl` to see the LLVM ir. You can use `rlc --ir file.rl -O2` if you want to see the optimized IR.

### Making IR more readable

When printing the RLC IR you can pass `--hide-positions` and `--hide-dl` to omit the module data layout and the module debug positions. That makes the IR slightly more readable.

### MLIR

I know you are looking for a explanation of how MLIR works since MLIR documentation is arcane. Good luck!

Once you figure it out: RLC uses MLIR as a mere intermediate rappresentation generator, and as a way to convert to LLVM IR. We use nothing about Dialect interoperability or tensorial stuff. Every MLIR operation we use(except when converting to LLVM IR) is custom made for RLC.

In practice what we get out MLIR is:
* the pipeline infrastructure
* the declarative way of creating new operation types
* the textual serializers for MLIR
* mlir-opt which runs single passes of mlir
* some basic type interfaces
* MLIR dataflow analysis

### Running single RLC passes

If you want to run a RLC pass in isolation after a particular step of the rlc pipeline, say the typechecking step, you can run

```
rlc --type-checked file.rl -o - | rlc-opt --PASS_NAME
```
