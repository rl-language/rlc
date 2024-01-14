# RUN: split-file %s %t
# RUN: rlc %t/source.rl -o %t3 -i %stdlib -i %t --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 4:3: error: Could not find matching function _function()

#--- source.rl
import to_include

fun function2():
  _function() 

#--- to_include.rl
fun _function():
  let y : Int

