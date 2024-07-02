# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act action() -> Action:
	frm counter = 0
	while true:
		act sub()
		counter = counter + 1	


fun main() -> Int:
	let frame = action()
	frame.sub()
	frame.sub()
	return frame.counter - 2
