fun<T> counter(T dc, Int x) -> Int:
	x = x + 1	
	if x == 5:
		return x
	else:
		return counter(dc, x)

fun main() -> Int:
	return counter(7, 0) - 5
