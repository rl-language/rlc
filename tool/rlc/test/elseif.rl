# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  if false:
    return -1
  else if false:
    return -2
  else if true:
    return 0
  else:
    return 1

