# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 10:1: error: Subaction statement must refer to a action, not a Something

cls Something:
	Int x

act outer() -> Outer:
	let val : Something
	subaction frame = val

