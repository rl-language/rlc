# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
	if !4 is Int:
		return -1
	else:
		return 0
