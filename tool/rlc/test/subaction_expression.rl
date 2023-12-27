# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act outer() -> Outer:
	subaction* inner_frame = inner()

act inner() -> Inner:
	let sum = 60
	while sum != -3:
		actions:
			act first(Int x, Int y)
			sum = x + y

			act second(Bool is_set)
			if is_set:
				sum = sum * -1

fun main() -> Int:
	let frame = outer()
	if frame.is_done():
		return 1
	frame.first(1, 2)
	if frame.is_done():
		return 2
	frame.second(true)
	if !frame.is_done():
		return 4
	return frame.inner_frame.sum + 3
