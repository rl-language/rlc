# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 5:1: error: Found recursive call path involving Action Declaration. This is not allowed since would actions frame of infinite size

act example(Int x) -> Example: 
	example(x)
