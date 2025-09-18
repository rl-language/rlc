# REQUIRES: has_ruby
# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/library.rb -i %stdlib --ruby
# RUN: ruby %t/to_run.rb


#--- source.rl
act play() -> Game:
    frm asd = 0
    act pick(Int x)
    asd = x

#--- to_run.rb
require_relative 'library'

pair = RLC.play
pair.pick 3
puts(pair.is_done)
raise "this is wrong" unless pair.asd == 3
