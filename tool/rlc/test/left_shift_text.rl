# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  if 4 << 2 != 16:
    return -1
  else:
    return 0

