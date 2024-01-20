# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act play() -> Play:
	frm z = 0
	actions:
		act mark(Int x, Int y) {x == 2}
		z = y	

		act other_mark(Int x, Int y) {y == 2}
		z = x

fun main() -> Int:
	let state = play()
	state.other_mark(3, 2)
	return state.z - 3
