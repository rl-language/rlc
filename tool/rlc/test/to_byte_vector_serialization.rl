import serialization.to_byte_vector

ent Struct2:
	Vector<Int> values

ent Struct1:
	Int asd
	Bool x
	Float[2] ras
	Vector<Struct2> inners

fun eq(Struct2 lhs, Struct2 rhs) -> Bool:
	let index = 0
	if lhs.values.size != rhs.values.size:
		return false
	while index < lhs.values.size:
		if lhs.values.data[index] != rhs.values.data[index]:
			return false
		index = index + 1
	return true

fun eq(Vector<Struct2> lhs, Vector<Struct2> rhs) -> Bool:
	let index = 0
	if lhs.size != rhs.size:
		return false
	while index < lhs.size:
		if !eq(lhs.data[index], rhs.data[index]):
			return false
		index = index + 1
	return true

fun eq(Struct1 lhs, Struct1 rhs) -> Bool:
	if lhs.asd != rhs.asd:
		return false
	if lhs.ras[0] != rhs.ras[0]:
		return false
	if lhs.ras[1] != rhs.ras[1]:
		return false
	if lhs.x != rhs.x:
		return false
	return eq(lhs.inners, rhs.inners)

fun main() -> Int:
	let var : Struct1
	var.asd = 2
	var.x = true
	var.ras[1] = 10.0
	var.ras[0] = 2.0
	let transformed = var.as_byte_vector()
	let result : Struct1
	from_byte_vector(result, transformed)
	return int(eq(var, result)) - 1
