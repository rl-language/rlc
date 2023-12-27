# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun fun_with_trailing_type() -> Void:
	let aasds = 0
	return

fun function():
	return


fun main() -> Int:
	function()
	fun_with_trailing_type()	
	return 0
