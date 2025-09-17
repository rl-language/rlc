# REQUIRES: has_ruby
# RUN: split-file %s %t

# RUN: rlc %t/source.rl -o %t/lib%sharedext -i %stdlib --shared
# RUN: rlc %t/source.rl -o %t/lib%libext -i %stdlib --compile

# RUN: rlc %t/source.rl -o %t/library.rb -i %stdlib --ruby
# RUN: ruby %t/to_run.rb


#--- source.rl
import collections.vector 
cls Pair:
    Vector<Int> x 

fun asd():
    let x : Pair
    x.x.size()

#--- to_run.rb
require_relative 'library'


pair = VectorTint64_tT.new
x = 2;
pair.append(x);
pair.append(x);
pair.append(x);
raise "this is wrong" unless pair.size == 3
