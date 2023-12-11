act action():
	act sub(Int x) {x == 0}

fun main() -> Int:
	let a = action()
	a.sub(1)
	return 0
