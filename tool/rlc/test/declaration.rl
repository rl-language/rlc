system call

fun a() -> Int:
	let a = 0
	let b = a + 4
	return b

fun main() -> Int:
	let x = a()
	return 0
