# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
	let counter = 4
	return [counter][0] - 4
