# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# THE POLICY WE USE NOW IS [min,max)

# a_true  : (11, MAX_INT)
# a_false : (MIN_INT, 11)

fun foo(Int a) -> Bool:
	if a > 10:
		return true
	return false

