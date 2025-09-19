# REQUIRES: has_ruby
# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/library.rb -i %stdlib --ruby
# RUN: ruby %t/to_run.rb


#--- source.rl
import collections.vector
import serialization.print

cls Context:
  Int x
  Int y

@classes
act sequence(ctx Context context) -> Sequence:
  frm accumulator : Vector<Int>
  while true:
    act add(Int z)
    accumulator.append(context.x + context.y + z)
    print(accumulator)

fun main() -> Int:
  let x : Context
  from_string(x, to_string(x))
  return 0


#--- to_run.rb
require_relative 'library'
ctx = RLC::Context.new
sec = RLC::sequence ctx
str = RLC::to_string ctx
raise "wrong" unless RLC::to_ruby_str(RLC::to_rlc_str(RLC::to_ruby_str(str))) == RLC::to_ruby_str(str)
