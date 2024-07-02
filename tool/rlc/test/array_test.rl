# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun main() -> Int:
	let array : Int[10]
	let i = 0
	while i < 10:
		array[i] = i * i
		i = i + 1
	let array2 : Int[10]
	array2 = array
	return array2[7] - 49
