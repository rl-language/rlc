# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
    let x = 0
    while x != 3:
      while x != 10:
        x = x + 1
        if x == 1:
            continue 
        else:
            break
      x = x + 1

    if x != 3:
        return -1

    while x != 13:
      x = x + 5
      continue

    if x != 13:
        return -1

    while x != 18:
      x = x + 5

    return x - 18

