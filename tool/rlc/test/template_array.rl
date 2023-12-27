# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

ent<T> ArrayVector:
	T[10] data
	Int size

fun<T> append(ArrayVector<T> vec, T new):
	vec.data[vec.size] = new 
	vec.size = vec.size + 1

fun<T> pop(ArrayVector<T> vec) -> T:
	vec.size = vec.size - 1
	return vec.data[vec.size]

fun main() -> Int:
	let array : ArrayVector<Int>
	array.append(1)
	array.append(2)
	array.append(3)
	array.pop()
	return array.pop() - 2
