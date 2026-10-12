# RUN: rlc %s -o %t -i %stdlib 2>&1 --expect-fail | FileCheck %s

act asd() -> Action:
  frm x : Int
  frm x : Int
  act dc()
  # CHECK: Redeclaration of variable x
  return

