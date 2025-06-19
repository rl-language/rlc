# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  if 16 >> 2 != 4:
    return -1
  else:
    return 0

