fun main() -> Int:
	let converted = __builtin_to_array(6)
	let unconverted = __builtin_from_array<Int>(converted)
	let converted2 = __builtin_to_array(true)
	let uncoverted2 = __builtin_from_array<Bool>(converted2)
	let converted3 = __builtin_to_array(7.0)
	let unconverted3 = __builtin_from_array<Float>(converted3)
	return int(unconverted3) - (unconverted + int(uncoverted2))
