fun fun_with_arg(Int arg, Float f, Bool b) -> Int:
	let a = arg - 4
	return a

fun main() -> Int:
	return fun_with_arg(4, 3.14, true)
