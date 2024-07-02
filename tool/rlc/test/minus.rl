# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
	let a = 5
	a = -a
	a = a + 5
	return a
