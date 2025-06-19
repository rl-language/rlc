# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

using Asd = Int | Float

act asd(frm Asd x) -> RollStat:
  if x is Int:
    x = x + 1
    act asd()

fun main() -> Int:
  let x : Asd
  x = 0
  let y = asd(x)
  y.asd()
  let value = y.x
  if value is Int:
    return value - 1
  return -1

