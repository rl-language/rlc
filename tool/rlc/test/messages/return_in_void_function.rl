# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 6:2: error: Return statement returns values incompatible with the function signature

fun function():
	return 0
