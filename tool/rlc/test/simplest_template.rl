# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

fun<G> pick_second(G asd, G asd2) -> G:
	return asd

fun main() -> Int:
	return pick_second(1, 2) - 1

