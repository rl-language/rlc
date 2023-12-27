# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

import collections.vector

fun main() -> Int:
	if (0 == 1):
		return 1
	let v : Vector<Int>
	return 0
