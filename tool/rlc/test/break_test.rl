# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
    let x = 0
    while x != 6:
      while x != 10:
        x = x + 5
        break
      x = x + 1

    if x != 6:
        return -1

    while x != 10:
      x = x + 5
      break

    if x != 11:
        return -1

    while x != 16:
      x = x + 5

    return x - 16
