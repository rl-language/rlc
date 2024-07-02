# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

trait<T> Trait:
	fun wasd(T x, Int y) -> Int
	fun rasd(T K)

fun wasd(Int x, Int y) -> Int:
	return x + y

fun rasd(Int K):
	wasd(K, 0)

fun main() -> Int:
	if 5.0 is Trait:
		return -1

	if 5 is Trait:
		return 0
	else:
		return -1
