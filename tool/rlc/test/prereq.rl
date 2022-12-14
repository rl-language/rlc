act example(Int X):
req X < 4
	X = X + 1
	act sub_action(Int Y)
	req Y < X

fun main() -> Int:
	let frame = example(2)
	return 0
