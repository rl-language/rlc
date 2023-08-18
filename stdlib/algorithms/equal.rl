trait<T> Comparable
	fun custom_equal(T lhs, T rhs) -> Bool

fun custom_equal(Int lhs, Int rhs) -> Bool:
	return lhs == rhs

fun custom_equal(Bool lhs, Bool rhs) -> Bool:
	return lhs == rhs

fun custom_equal(Byte lhs, Byte rhs) -> Bool:
	return lhs == rhs

fun custom_equal(Float lhs, Float rhs) -> Bool:
	return lhs == rhs

fun<T, Int X> custom_equal(T[X] lhs, T[X] rhs):
	let counter = 0
	while counter < X:
		if !(lhs == rhs):
			return false
		counter = counter + 1
	return true

# to implement
fun<T> equal(T lhs, T rhs):
	if lhs is Comparable:
		if rhs is Comparable:
			return custom_equal(lhs, rhs)
	for field of lhs
