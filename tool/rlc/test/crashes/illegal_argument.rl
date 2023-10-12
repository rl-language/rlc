act action():
	act subaction(Int x) {x == 0}

fun main() -> Int:
	let a = action()
	a.subaction(1)
	return 0
