# REQUIRES: has_ruby
# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/library.rb -i %stdlib --ruby
# RUN: ruby %t/to_run.rb


#--- source.rl
fun to_invoke() -> Int {true}:
  return 5

#--- to_run.rb
require_relative 'library'

raise "this is wrong" unless to_invoke == 5
