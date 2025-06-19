# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun fun_with_trailing_type():
  let aasds = 0
  return

fun function():
  return

fun main() -> Int:
  function()
  fun_with_trailing_type()
  return 0

