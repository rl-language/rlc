# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : (11,20)
# a_false : (20,MAX_INT) <- this is not entirely correct, it is missing the part where (a<10)

# but conceptually we cannot know it since that return false could be any other statement, and if no information
# is generated from the range in the other range it cannot be propagated.
# What we could do at the end is a normalization pass that fixes all the ranges (i.e. if i find a range which is bounded then the negation is unbounded)

fun foo(Int a) -> Bool:
	if a > 10:
		return a < 20
	return false
