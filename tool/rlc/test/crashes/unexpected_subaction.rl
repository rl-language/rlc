# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t
# XFAIL: *

act action() -> Action:
	act first()
	act second()

fun main() -> Int:
	let a = action()
	a.second()
	return 0
