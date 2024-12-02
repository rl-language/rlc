# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 6:13: error: Array access must have a integer index operand 

fun main() -> Int:
	return [11][[1, 4]]
