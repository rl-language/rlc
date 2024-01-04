# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

# These files are a series of examples meant to introduce you to the rulebook language.
# Each of these files is a valid rl program that can be run by pressing ctrl+shift+b.

# Here the simplest rl program, try running it, and then try edit the return value to 10 
# to see the program fail.

fun main() -> Int:
  let exit_code = 0
  return exit_code

# Open the other files, starting with `2.rl`, in this directory to see what are the 
# main features of the language.

# The `more_examples` folder contains other short programs currently used as tests 
# in the compiler that you can freely explore.
