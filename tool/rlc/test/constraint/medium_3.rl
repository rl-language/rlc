# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : (MIN_INT, MAX_INT)
# a_false : (10,11)

fun foo(Int a) -> Bool:
	if a > 10:
		return true
	else if a < 10:
		return true
	return false

