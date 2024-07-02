# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
	let a = false
	if !a:
		return 0
	return 1
