# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t

cls<T> ArrayVector:
	T[10] data
	Int size

	fun append(T new):
		self.data[self.size] = new 
		self.size = self.size + 1

	fun pop() -> T:
		self.size = self.size - 1
		return self.data[self.size]

fun main() -> Int:
	let array : ArrayVector<Int>
	array.append(1)
	array.append(2)
	array.append(3)
	array.pop()
	return array.pop() - 2
