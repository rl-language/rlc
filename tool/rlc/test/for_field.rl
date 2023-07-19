ent Inner:
	Int x2
	Int y2
ent Asd:
	Int x
	Inner y

fun<T> collect(T asd) -> Int:
	let result = 0

	for field of asd:
		if field is Int:
			result = result + field
		if field is Float:
			result = result + int(field)
		if field is Bool:
			result = result + int(field)
		result = result + collect(field)

	return result

fun main() -> Int:
	let var : Asd
	var.x = 1
	var.y.x2 = 3
	var.y.y2 = 5
	return collect(var) - 9
