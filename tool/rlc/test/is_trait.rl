# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

trait<T> SummableToInt:
	fun addInt(T x, Int y) -> Int

fun addInt(Float x, Int y) -> Int:
	return int(x) + y

fun<K> tryAddInt(K x, Int y) -> Int:
	if x is SummableToInt:
		return x.addInt(y)
	return y

fun main() -> Int:
	return tryAddInt(4.0, 3) -7
