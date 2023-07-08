ent<T> Vector:
	OwningPtr<T> content
	Int size
	Int capacity

fun<T> init(Vector<T> v):
	v.content = __builtin_malloc_do_not_use<T>(4)
	v.size = 0
	v.capacity = 4
	let counter = 0
	while counter < v.capacity:
		let new_elemt : T
		v.content[counter] = new_elemt
		counter = counter + 1

fun<T> drop(Vector<T> v):
	__builtin_free_do_not_use(v.content)
	v.size = 0
	v.capacity = 0

fun<T> append(Vector<T> v, T elem):
	v.content[v.size] = elem
	v.size = v.size + 1

fun<T> pop(Vector<T> v) -> T:
	v.size = v.size - 1
	return v.content[v.size]

fun main() -> Int:
	let vec : Vector<Int>
	vec.append(1)
	vec.append(2)
	vec.append(3)
	vec.pop()
	return vec.pop() - 2
