# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext
# XFAIL: *

fun main() -> Int:
	let array : Int[10]
	let a = array[10]   
	return 0
