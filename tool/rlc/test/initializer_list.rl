# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
	let list = [[1, 1], 
				[2, 2], 
				[3, 3]]
	return list[0][1] + list[1][1] - list[2][1]
