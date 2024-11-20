# RUN: rlc %s -o %t -i %stdlib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 8:1: error: Redefinition of action function named action 

act action() -> Action:
	act first()

act action() -> Actin:
	act first()	

fun main() -> Int:
	let state = action()
	return 0
