# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: error: Int is not allowed as action return type

act action() -> Int:
	return 5

