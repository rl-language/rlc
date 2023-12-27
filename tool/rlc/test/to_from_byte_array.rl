# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
	let converted = __builtin_to_array(6)
	let unconverted = __builtin_from_array<Int>(converted)
	let converted2 = __builtin_to_array(true)
	let uncoverted2 = __builtin_from_array<Bool>(converted2)
	let converted3 = __builtin_to_array(7.0)
	let unconverted3 = __builtin_from_array<Float>(converted3)
	let byte_array =  __builtin_to_array(true)
	let read = __builtin_from_array<Bool>(byte_array)
	if !read:
		return -1
	let byte_array2 =  __builtin_to_array(false)
	let read2 = __builtin_from_array<Bool>(byte_array2)
	if read2:
		return -2
	return int(unconverted3) - (unconverted + int(uncoverted2))
