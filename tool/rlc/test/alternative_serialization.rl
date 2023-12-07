import collections.vector
import serialization.to_byte_vector

fun main() -> Int:
	let asd : Int | Float
	asd = 3.4
	
	let serialized = asd.as_byte_vector()
	let result : Int | Float
	result.from_byte_vector(serialized) 

	if result is Float:
		return int(result - 3.4)
	else:
		return -1
