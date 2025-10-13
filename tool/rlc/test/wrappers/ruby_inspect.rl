# REQUIRES: has_ruby
# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/library.rb -i %stdlib --ruby
# RUN: ruby %t/to_run.rb


#--- source.rl
cls Pair:
    Int x 
    Int y

    fun to_invoke() -> Int {true}:
        return self.x + self.y

cls Outer:
    Pair inner

    fun to_invoke() -> Int {true}:
        return self.inner.x + self.inner.y

using Asd = Int | Outer

#--- to_run.rb
require_relative 'library'

raise "this is wrong" unless RLC::Outer.all_members == {inner: RLC::Pair}
raise "this is wrong" unless RLC::RLCAsd.all_alternatives == [Integer, RLC::Outer] 


