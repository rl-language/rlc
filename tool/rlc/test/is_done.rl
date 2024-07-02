# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act action() -> Action:
	act first()	
	act second()	

fun main() -> Int:
	let state = action()
	if is_done(state):
		return -1
	state.first()
	if is_done(state):
		return -2
	state.second()
	if is_done(state):
		return 0
	return 1
