act action_example(Int x) -> Action:
	let wasd = 4
	let asd = wasd + x

	act dont_care()
	asd = asd

fun main() -> Int:
	let coroutine_frame = action_example(6)
	return coroutine_frame.asd  - 10
