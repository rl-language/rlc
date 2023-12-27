# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

act action() -> Action:
	let Counter = 0
	while true:
		act sub()
		Counter = Counter + 1	


fun main() -> Int:
	let frame = action()
	frame.sub()
	frame.sub()
	return frame.Counter - 2
