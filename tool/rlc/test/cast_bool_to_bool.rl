# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun main() -> Int:
	return int(bool(false))
