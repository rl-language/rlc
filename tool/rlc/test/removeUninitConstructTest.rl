# RUN: rlc %s -o - -i %stdlib --flattened 

fun asd() -> Bool:
  return 1 == 1 and 2 == 2 or 3 == 3

