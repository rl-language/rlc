# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
	using T = type(6)
	let asd : T
	asd = 10
	return asd - 10
