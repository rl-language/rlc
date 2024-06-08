# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

cls<T> TemplatedEntity:
	T asd

fun main() -> Int:
	let var : TemplatedEntity<Int>
	var.asd = 2	
	return var.asd - 2
