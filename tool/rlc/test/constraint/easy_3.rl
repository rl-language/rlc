# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7,0)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : (MIN_INT, 10)
# a_false : (10, MAX_INT)
# b_true  : (11, MAX_INT)
# b_false : (MIN_INT, 11)

fun foo(Int a, Int b) -> Bool:
	if b > 10:
		if a < 10:
			return true
	return false

