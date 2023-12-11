ent Asd:
	Int x

fun ret_x(Asd val) -> ref Int:
	return val.x

fun main() -> Int:
	let var : Asd
	var.x = 10
	let refvar = ret_x(var)
	var.x = 20
	return refvar - 20
