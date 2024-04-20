# RUN: rlc %s -o %t -i %stdlib
# RUN: %t

# Example found also in the branch README.md

fun main() -> Int:
	let exit_code = trivial(7)
	if exit_code == true:
		return 0
	else:
		return -1

fun trivial(Int a) -> Bool:
	if a > 0:
		let temp1 = a+2 
		if temp1 < 10:
			return true
	return false
