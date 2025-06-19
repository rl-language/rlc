# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

const global = 3

fun main() -> Int:
  let x : Int[global]
  if global == 3:
    return 0
  else:
    return -1

