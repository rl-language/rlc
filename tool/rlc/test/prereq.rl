# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act example(Int X) -> Example {X < 4}: 
	X = X + 1
	act sub_action(Int Y) {Y < X}
	X = X

fun main() -> Int:
	let frame = example(2)
	return 0
