# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

fun foo(Int a) -> Bool:
	if a > 0:
		return a < 10
	return false
