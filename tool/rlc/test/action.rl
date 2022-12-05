act action_example(Int asd):
	let wasd = 4
	asd = wasd + asd

fun main() -> Int:
	let coroutine_frame = action_example(6)
	return coroutine_frame.asd  - 10
