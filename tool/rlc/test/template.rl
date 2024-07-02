# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

trait<T> Addable:
	fun add(T i, T i2) -> T

fun<Addable T> timesTwo(T i) -> T:
	return add(i, i)

fun add(Int i, Int i2) -> Int:
	return i + i2

fun main() -> Int:
	return timesTwo(1) - 2
