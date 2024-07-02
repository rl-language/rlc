# RUN: rlc %s -o %t -i %stdlib --sanitize
# RUN: %t%exeext

import collections.vector

act play(Int seed) -> Play:
	let board : Vector<Int>

	frm over = false

	while !over:
		actions:
			act not_quit(Int x)
			over = false	
			
			act quit(Int y)
			over = true

fun main() -> Int:
    let state = play(4)
    state.not_quit(2)
    state.quit(2)
    return 0
