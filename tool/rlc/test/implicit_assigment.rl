# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent asd:
	Int first
	Int second

fun main() -> Int:
	let e1 : asd
	let e2 : asd
	e1.second = 4
	e2 = e1
	return e2.second - 4	
