import collections.vector

act play(Int seed) -> Play:
	let board : Vector<Int>

	let over = false

	while !over:
		actions:
			act not_quit()
			over = false	
			
			act quit()
			over = true


fun main() -> Int:
	let state = play(5)
	state.quit()
	return int(state.over) - 1
