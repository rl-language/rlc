# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
  let ptr : OwningPtr<Int>
  let ptr2 : OwningPtr<Int>
  if ptr == ptr2:
    return 0
  else:
    return 1

