# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun<T> returnDoubleIfInt(T a) -> T:
	let to_return : T
	if a is Int:
		if to_return is Int:
			to_return = a + a
	return to_return

fun main() -> Int:
	return returnDoubleIfInt(2) - 4
