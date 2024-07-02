# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
# XFAIL: *

act action() -> Action:
	act sub(Int x) {x == 0}

fun main() -> Int:
	let a = action()
	a.sub(1)
	return 0
