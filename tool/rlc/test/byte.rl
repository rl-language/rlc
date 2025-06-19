# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  let var = byte(8)
  var = var - byte(7)
  var = var - byte(true)
  return int(var)

