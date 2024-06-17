# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# NORMALLY HERE GO THE UNCORRECT TESTS WHICH I THINK FOR THE MOMENT ARE NOT FIXABLE

# a_true  : (1, MAX_INT)
# a_false : (MIN_INT,10) <- this is not entirely correct

fun foo(Int a) -> Bool:
	if a > 10:
        return true
	else if a > 0:
		return true
	return false

