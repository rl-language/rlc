# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

import collections.vector

fun main() -> Int:
	let asd : Int | Float | Bool | Vector<Int>
	asd = false
	if asd is Float:
		return -2 
	asd = 3
	if asd is Int:
		return asd - 3
	return -1
