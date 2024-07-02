# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun with_if_else() -> Int:
	if 4 == 4:
		return 0
	else:
		return 1

fun main() -> Int:
	return with_if_else()
