# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

act example(frm Int x) -> Example {x < 4}: 
	x = x + 1
	act sub_action(Int y) {y < x}
	x = x

fun main() -> Int:
	let frame = example(2)
	return 0
