# RUN: rlc %s -o %t -i %stdlib 
# RUN: %t%exeext

cls<T> Vector:
	OwningPtr<T> content
	Int size
	Int capacity

	fun init():
		self.content = __builtin_malloc_do_not_use<T>(4)
		self.size = 0
		self.capacity = 4
		let counter = 0
		while counter < self.capacity:
			let new_elemt : T
			self.content[counter] = new_elemt
			counter = counter + 1

	fun drop():
		let counter = 0
		while counter < self.capacity:
			__builtin_destroy_do_not_use(self.content[counter])
			counter = counter + 1
		__builtin_free_do_not_use(self.content)
		self.size = 0
		self.capacity = 0

	fun append(T elem):
		self.content[self.size] = elem
		self.size = self.size + 1

	fun pop() -> T:
		self.size = self.size - 1
		return self.content[self.size]

cls VectorContainer:
	Vector<Int> asd


fun main() -> Int:
	let vec : Vector<Int>
	vec.append(1)
	vec.append(2)
	vec.append(3)
	vec.pop()
	return vec.pop() - 2
