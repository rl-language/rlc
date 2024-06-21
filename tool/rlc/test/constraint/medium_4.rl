# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

fun main() -> Int:
	let exit_code = foo(7)
	if exit_code == true:
		return 0
	else:
		return -1

# a_true  : (11,19)
# a_false : (MIN_INT, MAX_INT)

fun foo(Int a) -> Bool:
	if a > 10:
        if a < 20:
            return true
        # we want to be conservative, this will be (15, ?) -> (10, ?) (because of line 15) 
        # -> (MIN_INT,MAX_INT) because of the join with the false branch (else a <=10)
        if a > 15:
            return false
	return false

