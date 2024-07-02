# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun return_true() -> Bool:
	return 4 * 12 == 48

fun main() -> Int:
	let yes = return_true() and true
	return 0
