# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import collections.vector
import serialization.to_byte_vector

fun main() -> Int:
	let asd : Int | Float
	asd = 3.4
	
	let serialized = as_byte_vector(asd)
	let result : Int | Float
	from_byte_vector(result, serialized) 

	if result is Float:
		return int(result - 3.4)
	else:
		return -1
