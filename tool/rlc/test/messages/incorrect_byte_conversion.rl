# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 7:20: error: Type argument of __builtin_from_array must be a primitive type, not Booi

fun main() -> Int:
	let converted2 = __builtin_to_array(true)
	let uncoverted2 = __builtin_from_array<Booi>(converted2)
	return 0 
