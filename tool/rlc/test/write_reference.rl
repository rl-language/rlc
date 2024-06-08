# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

cls Outer:
	Int inner

fun take_ref(Outer val) -> ref Int:
	return val.inner

fun main() -> Int:
	let var : Outer
	var.inner = 4
	take_ref(var) = 3
	return var.inner - 3

