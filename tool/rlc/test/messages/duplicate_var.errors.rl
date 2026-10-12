# RUN: rlc %s -o %t -i %stdlib 2>&1 --expect-fail | FileCheck %s

fun asd():
  let x : Int
  let x : Int
  # CHECK: Redeclaration of variable x
  return

