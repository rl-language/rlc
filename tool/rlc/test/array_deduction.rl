fun<X, Int T> f(X[T] x) -> X:
	if x is Int[T]:
		return x[0] + T
	else:
		return x[0]

fun main() -> Int:
	let x : Int[4]
	x[0] = 5
	return f(x) - 9
