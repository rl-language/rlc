# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : (11,20)
# a_false : (MIN_INT,MAX_INT)

# This case now becomes analogous to medium_6.rl

fun foo(Int a) -> Bool:
	if a > 10:
		return a < 20
	return false
