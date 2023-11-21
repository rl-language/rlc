act action():
	act first()
	act second()

fun main() -> Int:
	let a = action()
	a.second()
	return 0
