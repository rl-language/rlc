# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act action() -> State:
    actions:
      act first()
        act second()

fun main() -> Int:
  let state = action() 
  if can state.second():
        return -1
  return 0
