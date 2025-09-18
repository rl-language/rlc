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

act sequence(ctx Context context) -> Sequence:
    frm accumulator : Vector<Int>  # drop this and Sequence will become trivially constructible (that is, no mallocs triggered)
    while true:
        act add(Int z)
        accumulator.append(context.x + context.y + z)
        print(accumulator)

#--- to_run.rb
require_relative 'library'

ctx = RLC::Context.new
ctx.x = 3
ctx.y = 2
state = RLC.sequence ctx
if state.can_add(ctx, 10)
    state.add(ctx, 10)
end
