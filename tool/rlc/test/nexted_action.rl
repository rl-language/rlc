act guess():
	act p1_choise(Int secret)

	while true:
		act p2_choise(Int guess)	

		if guess == secret:
			let result = secret + guess
			return

fun main() -> Int:
	let struct = guess()
	if struct.is_done():
		return 1
	struct.p1_choise(1)
	struct.p2_choise(3)
	struct.p2_choise(4)
	struct.p2_choise(1)
	return struct.result - 2
