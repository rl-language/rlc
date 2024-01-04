# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent some_entity:
	Int field
	Int field2

fun main() -> Int:
	let var : some_entity
	let var2 : Int
	var.field2 = 10
	var.field2 = var.field2 - 10
	return var.field2
