# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
	using T = Int
	let asd : T
	asd = 10
	return asd - 10

