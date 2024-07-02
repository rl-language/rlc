# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun while_function(Int a) -> Int:
	while (4 > 5):
		let b = 8

	while (4 < 5):
		return 0

	return 1

fun main() -> Int:
	return while_function(6)
