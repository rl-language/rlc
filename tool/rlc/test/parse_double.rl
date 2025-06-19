# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import string

fun main() -> Int:
  let parsed : Float
  if parse_string(parsed, " 34.4 "s, 0) and parsed == 34.4:
    return 0
  else:
    return 1

