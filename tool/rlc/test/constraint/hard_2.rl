# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# THIS WORKS WELL BUT IS TRICKY TO UNDERSTAND WHY the result is not exactly what i expect
# The problem is that at first it recognises it correctly, but then when it starts to perform
# MEET operations (which for the moment are joins) it will meet the path of the basic blocks 
# at lines 21 and 23 because both have as predecessor the basic block at line 20

# a_true  : (MIN_INT, 11) <- should be a 10 not 11 
# a_false : (MIN_INT, MAX_INT)

fun foo(Int a) -> Bool:
	if a > 10:
        if a < 20:
            return false
	else if a < 10:
		return true
	return false

