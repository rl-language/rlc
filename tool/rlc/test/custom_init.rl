# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent Example:
	Int f1

fun init(Example e):
	e.f1 = 5	

fun main() -> Int:
	let asd : Example
	return asd.f1 - 5
