# RUN: rlc --expect-fail --print-ir-on-failure=false %s -o %t -i %stdlib 2>&1 | FileCheck %s

# CHECK: 10:14: error: No known value x

act outer() -> Outer:
	inner()

act inner() -> Inner:
	let to_return : Int
	to_return = x

fun main() -> Int:
	return 0 
