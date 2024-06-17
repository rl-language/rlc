# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : (MIN_INT, MAX_INT)
# a_false : (10,10)

fun foo(Int a) -> Bool:
	if a > 10:
        return true
	# NB can be both an if or an else if -> does not matter
	else if a < 10:
		return true
	return false

