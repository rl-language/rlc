import collections.vector

fun main() -> Int:
	let v : Vector<Int>
	v.append(1)
	v.append(2)
	v.append(3)
	v.pop()
	return v.pop() - 2

