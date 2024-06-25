# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : the range will be a join of (11,19) and (MIN_INT, -11) -> (MIN_INT, 19)
# a_false : the range will be a join of (10, MAX_INT) and (-10, MAX_INT) -> (-10, MAX_INT)

fun foo(Int a) -> Bool:
	if a > 10:
        if a < 20:
            return true
	if a < -10:
		return true
	return false

