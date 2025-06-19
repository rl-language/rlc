# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import enum_utils
import serialization.print

enum Example:
  first
  second

fun main() -> Int:
  let check : Example
  if to_string(Example::first) != "first":
    return -1
  if to_string(Example::second) != "second":
    return -2
  if !from_string(check, " second"s) or check.value != 1:
    return -4
  if !from_string(check, "first"s) or check.value != 0:
    return -3
  return 0

