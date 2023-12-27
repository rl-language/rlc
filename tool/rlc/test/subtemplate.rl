# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

fun<T> identity(T asd) -> T:
	return asd

fun<G> pick_second(G asd, G asd2) -> G:
	return identity(asd2)

fun main() -> Int:
	return pick_second(1, 2) - 2
