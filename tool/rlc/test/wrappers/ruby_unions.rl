# REQUIRES: has_ruby
# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/library.rb -i %stdlib --ruby
# RUN: ruby %t/to_run.rb


#--- source.rl
cls Pair:
    Int | Float x 


cls Outer:
    Pair inner

#--- to_run.rb
require_relative 'library'

outer = RLC::Outer.new;
outer.inner.x.assign(2.2);
if outer.inner.x[1] != nil
    return -10;
end
raise "this is wrong" unless outer.inner.x[1] - 2.2

